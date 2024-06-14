#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include "shell_functions.h"

#define CWD_BUF_LEN 256
#define FILE_BUF_LEN 1024

// this will be used for future features
// returns true if dir contains file, otherwise, false
int file_exists(char* dir_name, char* file_name){
    DIR* d = opendir(dir_name);
    struct dirent* file;
    while((file = readdir(d))){
        if (!strcmp(file->d_name, file_name))
            return 1;
    }
    return 0;
}

// changes the current working directory
void change_dir(char* path){
    if (chdir(path))
        printf("Invalid path: \"%s\"\n", path);
}

// runs the LS command
// TODO: allow this to optionally display hidden files
void list_dir(char* path){
    DIR* d = opendir(path);
    struct dirent* file;
    printf("\t%s:\n", path);
    while ((file = readdir(d))){
        if (file->d_name[0] != '.')
            printf("\t\t%s\n", file->d_name);
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
    if (!access(path, F_OK)){
        printf("Could not create the file \"%s\". Does it already exist?\n", path);
        return;
    }
    fopen(path, "w");
}

// prints the contents of a file
void print_file(char* path){
    // ensure the file can be read
    if (access(path, F_OK)){
        printf("Failed to open the file: \"%s\" does it exist?\n", path);
        return;
    }
    FILE* file = fopen(path, "r");
    char buf[FILE_BUF_LEN];
    while (fgets(buf, FILE_BUF_LEN, file))
        printf("%s", buf);
    puts("\n\n");   
}

// exits the program
void exit_shell(){
    printf("Bye, %s\n", getlogin());
    exit(0);
}