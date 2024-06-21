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

char* command_list[COMMAND_COUNT] = {"cd", "ls", "cwd", "create", "created", "del", "rmdir", "cp", "mv", "echo", "echof", "clear", "help", "exit"};

enum commands {CD=0, LS, CWD, CREATE, CREATE_D, DEL, RMDIR, CP, MV, ECHO, ECHOF, CLEAR, HELP, EXIT};

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
int handle_command(char** raw_args, char** args, char** flags, int arg_count, int flag_count){
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
    switch (command_no){
        case CD:
            char* path;
            if (arg_count == 1)
                asprintf(&path, "/home/%s/", getlogin());
            else
                path = args[1];
            if (change_dir(path))
                printf("error: %s: directory not found\n", path);
            break;
        case LS:
            if (arg_count == 1)
                getcwd(path, BUF_LEN);
            else
                path = args[1];
            if (list_dir(path, flags, flag_count))
                printf("error: %s: directory could not be read\n", path);
            break;
        case CWD:
            print_cwd();
            break;
        case CREATE:
            if (arg_count < 2)
                puts("error: create: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (create_file(args[i]))
                        printf("error: %s: cannot create file\n", args[i]);
            }
            break;
        case CREATE_D:
            if (arg_count < 2)
                puts("error: created: please provide a directory opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (create_dir(args[i]))
                        printf("error: %s: cannot create directory\n", args[i]);
            }
            break;
        case DEL:
            if (arg_count < 2)
                puts("error: del: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (delete_file(args[i]))
                        printf("error: %s: failed to delete the file, does it exist?\n", args[i]);
            }
            break;
        case RMDIR:
            if (arg_count < 2)
                puts("error: rmdir: please provide a path operand");
            else{
                for (int i = 1; i < arg_count; i++)
                    switch (delete_directory(args[i], flags, flag_count)){
                        case 0:
                            break;
                        case 1:
                            printf("error: %s: could not delete the directory (run with the -r flag to recursively delte the directory)\n", args[i]);
                            break;
                        case 2: 
                            printf("error: %s: could not delete directory\n", args[i]);
                            break;
                    }

            }  
            break;
        case CP:
            if (arg_count != 3)
                puts("error: cp: command accepts exactly two arguments");
                break;
            if (copy_file(args[1], args[2]))
                printf("error: %s: could not read the file\n", args[1]);
            break;
        case MV:
            if (arg_count != 3){
                puts("error: mv: command accepts exactly two arguments");
                break;
            }
            switch (move_file(args[1], args[2])){
                case 0:
                    break;
                case 1:
                    printf("error: %s: could not access file\n", args[1]);
                    break;
                case 2:
                    printf("error: %s: could not delete source file\n", args[1]);
                    break;
            }
            break;
        case ECHO:
            int error = echo(args + 1, arg_count - 1);
            switch (error){
                case 0:
                    break;
                case 1:
                    puts("error: echo: insertion operator missing right operand\n");
                    break;
                case 2:
                    printf("error: cannot open file\n");
                    break;
            }
            break;
        case ECHOF:
            if (arg_count < 2)
                puts("error: echof: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    if (print_file(args[i]))
                        printf("error: %s: cannot read file\n", path);
            }
            break;
        case CLEAR:
            system("clear");
            break;
        case HELP:
            print_help();
            break;
        case EXIT:
            return 1;
        default:
            if (strcmp("", command))
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
    int arg_count = parse_args(input, raw_args);
    // parse the arguments and flags
    int flag_count = parse_flags(raw_args, args, flags, arg_count);
    arg_count -= flag_count;
    int exit = handle_command(raw_args, args, flags, arg_count, flag_count);
    if (!exit)
        input_loop();
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