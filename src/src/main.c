#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "shell_functions.h"

#define BUF_LEN 1024
#define MAX_ARGS 256
#define MAX_FLAGS 256
#define UNAME_LEN 32
#define _GNU_SOURCE

// associate all commands with their respective flags (if they have any)
char* ls_flags[1] = {"-a"};
char* rmdir_flags[2] = {"-r", "-f"};

char* command_list[COMMAND_COUNT] = {"cd", "ls", "cwd", "create", "created", "del", "rmdir", "cp", "mv", "echo", "echof", "run", "clear", "help", "exit"};

enum commands {CD=0, LS, CWD, CREATE, CREATE_D, DEL, RMDIR, CP, MV, ECHO, ECHOF, RUN, CLEAR, HELP, EXIT};

// ensures that all flags for a given command are correct
// TODO: Find if there is a way to do this that's more efficient than O(n^2)
int validate_flags(char* command_name, char** valid, int valid_count, char** provided, int provided_count){
    for (int i = 0; i < provided_count; i++){
        int valid_flag = 0;
        for (int j = 0; j < valid_count; j++){
            if (strcmp(valid[j], provided[i]) == 0){
                valid_flag = 1;
                break;
            }
        }
        if (!valid_flag){
            printf("error: %s: unrecognized flag %s\n", command_name, provided[i]);
            return FLAG_ERR;
        }
    }
    return OK;
}

// returns a pointer to the file to be written to, and overwrites arg_count if the redirection operator is present
FILE* parse_redirect(char** args, int* arg_count){
    int redirect_pos = -1;
    for (int i = 0; i < *arg_count; i++){
        if (!strcmp(args[i], ">")){
            redirect_pos = i;
            break;
        }
    }
    if (redirect_pos == -1)
        return stdout;
    else{
        *arg_count = redirect_pos;
        if (!args[redirect_pos + 1]){
            return NULL;
        }
        FILE* out = fopen(args[redirect_pos + 1], "w");
        return out;
    }
}


// takes the string of user input and writes it as an array of arguments
int parse_args(char* input, char** args){
    size_t arg_count = 0;
    char* arg;
    for (int i = 0; i < MAX_ARGS; i++){
        arg = strsep(&input, " ");
        if (arg == NULL)
            return arg_count;
        else{
            args[i] = arg;
            arg_count++; 
        }
    }
}

// takes arguments and seperates the flags from the other arguments, returns the number of flags
int parse_flags(char** raw_args, char** args, char** flags, int arg_count){
    int arg_pos = 0;
    int flag_pos = 0;
    char* tmp;
    for (int i = 0; i < arg_count; i++){
        tmp = raw_args[i];
        if (tmp[0] == '-'){
            flags[flag_pos] = tmp;
            flag_pos++;
        }
        else{
            args[arg_pos] = tmp;
            arg_pos++;
        }
    }
    return flag_pos;
}

// takes the arguments provided and runs the appropriate function
int handle_command(char** raw_args, char** args, char** flags, int raw_arg_count, int arg_count, int flag_count, FILE* out_file){
    // determine the command code
    char* command = args[0];
    size_t command_no = -1;
    for (int i = 0; i < COMMAND_COUNT; i++){
        if (strcmp(command, command_list[i]) == 0){
            command_no = i;
            break;
        }
    }
    // run the appropriate command
    char path[BUF_LEN];
    char* valid_flags;
    switch (command_no){
        case CD:
            if (validate_flags("cd", NULL, 0, flags,flag_count))
                break;
            // determine which directory to switch to
            if (arg_count == 1)
                sprintf(path, "/home/%s/", getlogin());
            else
                strcpy(path, args[1]);
            // change the directory, or display an error if we failed
            if (change_dir(path))
                printf("error: %s: directory not found\n", path);
            break;
        case LS:
            // validate the provided flags
            if (validate_flags("ls", ls_flags, 1, flags,flag_count))
                break;
            // determine the directory to list
            if (arg_count > 1)
                strcpy(path, args[1]);
            else
                getcwd(path, BUF_LEN);
            // list the directory's contents
            if (list_dir(path, flags, flag_count, out_file))
                printf("error: %s: could not list the provided directory (does it exist?)\n", path);
            break;           
        case CWD:
            if (validate_flags("cwd", NULL, 0, flags, flag_count))
                break;
            print_cwd(out_file);
            break;
        case CREATE:
            if (validate_flags("create", NULL, 0, flags, flag_count))
                break;
            if (arg_count < 2)
                puts("error: create: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (create_file(args[i]))
                        printf("error: %s: cannot create file\n", args[i]);
            }
            break;
        case CREATE_D:
            if (validate_flags("created", NULL, 0, flags, flag_count))
                break;
            if (arg_count < 2)
                puts("error: created: please provide a directory opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (create_dir(args[i]))
                        printf("error: %s: cannot create directory\n", args[i]);
            }
            break;
        case DEL:
            if (validate_flags("del", NULL, 0, flags, flag_count))
                break;
            if (arg_count < 2)
                puts("error: del: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (delete_file(args[i]))
                        printf("error: %s: failed to delete the file, does it exist?\n", args[i]);
            }
            break;
        case RMDIR:
            if (validate_flags("rmdir", rmdir_flags, 2, flags, flag_count))
                break;
            if (arg_count < 2)
                puts("error: rmdir: please provide a path operand");
            else{
                for (int i = 1; i < arg_count; i++)
                    switch (delete_directory(args[i], flags, flag_count)){
                        case OK:
                            break;
                        case ERR_1:
                            printf("error: %s: could not delete the directory (run with the -r flag to recursively delte the directory)\n", args[i]);
                            break;
                        case ERR_2: 
                            printf("error: %s: could not delete directory\n", args[i]);
                            break;
                        case ERR_3:
                            puts("directory not deleted");
                            break;
                        case ERR_4:
                            puts("unrecognized response");
                            break;
                    }
            }  
            break;
        case CP:
            if (validate_flags("cp", NULL, 0, flags, flag_count))
                break;
            if (arg_count != 3)
                puts("error: cp: command accepts exactly two arguments");
                break;
            if (copy_file(args[1], args[2]))
                printf("error: %s: could not read the file\n", args[1]);
            break;
        case MV:
            if (validate_flags("mv", NULL, 0, flags, flag_count))
                break;
            if (arg_count != 3){
                puts("error: mv: command accepts exactly two arguments");
                break;
            }
            switch (move_file(args[1], args[2])){
                case OK:
                    break;
                case ERR_1:
                    printf("error: %s: could not access file\n", args[1]);
                    break;
                case ERR_2:
                    printf("error: %s: could not delete source file\n", args[1]);
                    break;
            }
            break;
        case ECHO:
            if (validate_flags("echo", NULL, 0, flags, flag_count))
                break;
            echo(args + 1, arg_count - 1, out_file);
            break;
        case ECHOF:
            if (validate_flags("echof", NULL, 0, flags, flag_count))
                break;
            if (arg_count < 2)
                puts("error: echof: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (print_file(args[i], out_file))
                        printf("error: %s: cannot read file\n", path);
            }
            break;
        case RUN:
            if (arg_count < 2)
                puts("error: run: please provide a binary path");
            else{
                if (exec_bin(args[1], out_file) != OK)
                    printf("error: %s: unrecognized command", args[1]);
            }
            break;
        case CLEAR:
            if (validate_flags("clear", NULL, 0, flags, flag_count))
                break;
            system("clear");
            break;
        case HELP:
            if (validate_flags("help", NULL, 0, flags, flag_count))
                break;
            print_help(out_file);
            break;
        case EXIT:
            if (validate_flags("exit", NULL, 0, flags, flag_count))
                break;
            return 1;
        default:
            if (strcmp("", command) && strcmp("test", command))
                printf("error: %s: unrecognized file or command\n", command);
            break;
    }
    return 0;
}

// recieves and processes user input, then handles the given command
void input_loop(){
    char input[BUF_LEN];
    char* raw_args[MAX_ARGS];
    char* args[MAX_ARGS];
    char* flags[MAX_FLAGS];
    char uname[UNAME_LEN];
    printf("%s > ", getlogin());
    // get the user's arguments and strip the newline
    fgets(input, BUF_LEN, stdin);
    input[strlen(input) - 1] = 0;
    int raw_arg_count = parse_args(input, raw_args);
    // parse the arguments and flags
    int flag_count = parse_flags(raw_args, args, flags, raw_arg_count);
    int arg_count = raw_arg_count - flag_count;
    // parse the output file 
    FILE* out_file = parse_redirect(args, &arg_count);
    // remove the output file from the raw args if provdied
    if (out_file != stdin)
        raw_arg_count -= find_str(">", raw_args, raw_arg_count);
    if (!out_file){
        puts("Invalid output path!");
        input_loop();
    }
    else{
        int exit = handle_command(raw_args, args, flags, raw_arg_count, arg_count, flag_count, out_file);
        if (out_file != stdout)
            fclose(out_file);
        if (!exit)
            input_loop();
    }
}

// prevent escape with an interup
void interupt_handler (int _){
    printf("%s> ", getlogin());
}

int main(){
    signal(SIGINT, interupt_handler);
    signal(SIGTSTP, interupt_handler);
    system("clear");
    input_loop();
}