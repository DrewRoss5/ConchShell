#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "shell_functions.h"

#define BUF_LEN 1024
#define MAX_ARGS 32
#define UNAME_LEN 32
#define COMMAND_COUNT 5
#define _GNU_SOURCE

char* command_list[COMMAND_COUNT] = {"cd", "ls", "cwd", "create", "exit"};

enum commands {CD=0, LS, CWD, CREATE, EXIT};

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

int handle_command(char** args, int arg_count){
    char* command = args[0];
    size_t command_no = -1;
    // determine the command code
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
            list_dir(path);
            break;
        case CWD:
            print_cwd();
            break;
        case CREATE:
            if (arg_count < 2)
                puts("This command takes a minimum of one argument");
            else{
                for (int i = 1; i < arg_count; i++)
                    create_file(args[i]);
            }
            break;
        case EXIT:
            exit_shell();
            break;
        default:
            printf("Unrecognized Command: %s\n", command);
            break;
    }
}

void input_loop(){
    char input[BUF_LEN];
    char* args[MAX_ARGS];
    char uname[UNAME_LEN];
    // get and display the current working directory
    printf("%s > ", getlogin());
    // get the user's arguments and strip the newline
    fgets(input, BUF_LEN, stdin);
    input[strlen(input) - 1] = 0;
    int arg_count = parse_args(input, args);
    handle_command(args, arg_count);
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

