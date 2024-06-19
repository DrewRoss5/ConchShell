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
#define COMMAND_COUNT 11
#define _GNU_SOURCE

char* command_list[COMMAND_COUNT] = {"cd", "ls", "cwd", "create", "created", "del", "rmdir", "echo", "echof", "clear", "exit"};

enum commands {CD=0, LS, CWD, CREATE, CREATE_D, DEL, RMDIR, ECHO, ECHOF, CLEAR, EXIT};

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
            change_dir(path);
            break;
        case LS:
            if (arg_count == 1)
                getcwd(path, BUF_LEN);
            else
                path = args[1];
            list_dir(path, flags, flag_count);
            break;
        case CWD:
            print_cwd();
            break;
        case CREATE:
            if (arg_count < 2)
                puts("error: create: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    create_file(args[i]);
            }
            break;
        case CREATE_D:
            if (arg_count < 2)
                puts("error: created: please provide a directory opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    create_dir(args[i]);
            }
            break;
        case DEL:
            if (arg_count < 2)
                puts("error: del: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    delete_file(args[i]);
            }
            break;
        case RMDIR:
            if (arg_count < 2)
                puts("error: rmdir: please provide a path operand");
            else{
                for (int i = 1; i < arg_count; i++)
                    delete_directory(args[i], flags, flag_count);
            }
            break;
        case ECHO:
            echo(args + 1, arg_count - 1);
            break;
        case ECHOF:
            if (arg_count < 2)
                puts("error: echof: please provide a file opperand");
            else{
                for (int i = 1; i < arg_count; i++)
                    print_file(args[i]);
            }
            break;
        case EXIT:
            exit_shell();
            break;
        case CLEAR:
            system("clear");
            break;
        default:
            if (strcmp("", command))
                printf("error: %s: unrecognized file or command\n", command);
            break;
    }
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
    handle_command(raw_args, args, flags, arg_count, flag_count);
    input_loop();
}

// prevent escape with an interup
void interupt_handler (int _){
    puts("\n");
    input_loop();
}

int main(){
    signal(SIGINT, interupt_handler);
    signal(SIGTSTP, interupt_handler);
    system("clear");
    input_loop();
}