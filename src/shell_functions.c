#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include "shell_functions.h"

#define CWD_BUF_LEN 256

// changes the current working directory
void change_dir(char* path){
    if (chdir(path))
        printf("Invalid path: \"%s\"\n", path);
}

// runs the LS command
// TODO: allow this to optionally display hidden files
void list_dir(char* path){
    DIR* d = opendir(path);
    struct dirent* dir;
    printf("\t%s:\n", path);
    while ((dir = readdir(d))){
        if (dir->d_name[0] != '.')
            printf("\t\t%s\n", dir->d_name);
    }
}

// displays the current working directory
void print_cwd(){
    char cwd[CWD_BUF_LEN];
    getcwd(cwd, CWD_BUF_LEN);
    printf("%s\n", cwd);
}

// creates a new file if it does not already exist
void create_file(char* path){
    if (!access(path, F_OK))
        printf("Could not create the file \"%s\". Does it already exist?\n", path);
    fopen(path, "w");
}

// exits the program
void exit_shell(){
    printf("Bye, %s\n", getlogin());
    exit(0);
}