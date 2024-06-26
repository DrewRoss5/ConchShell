#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "shell_functions.h"

#define PATH_LEN 256
#define FILE_BUF_LEN 1024
#define _GNU_SOURCE

// empties stdin (this function is not strictly nessecary, but looks less ugly than calling the while loop in the function)
void flush_stdin() {while (fgetc(stdin) != '\n');}

// this will be used for future features
// returns true if dir contains file, otherwise, false
int file_exists(char* dir_name, char* file_name){
    DIR* d = opendir(dir_name);
    struct dirent* file;
    while((file = readdir(d))){
        if (!strcmp(file->d_name, file_name))
            return ERR_1;
    }
    return OK;
}

// returns the position of the first instance of a particular string in an array of strings. Or -1 if it's not present
int find_str(char* target, char** arr, int size){
    int pos = -1;
    for (int i = 0; i < size; i++){
        if (strcmp(arr[i], target) == 0){
            pos = i;
            break;
        }
    }
    return pos;
}

// changes the current working directory
int change_dir(char* path){
    if (chdir(path))
        return ERR_1;
    return OK;
}

// runs the LS command
int list_dir(char* path, char** flags, int flag_count, FILE* out_file){
    // determine if hiddent directoies should be shown
    int show_all = (find_str("-a", flags, flag_count) == -1) ? 0 : 1;
    // list the directory
    struct dirent* entry;
    DIR* dir = opendir(path);
    if (!dir)
        return ERR_1;
    while ((entry = readdir(dir)) != NULL){
        if (entry->d_name[0] != '.' || show_all)
            fprintf(out_file, "%s\n", entry->d_name);
    }
    closedir(dir);
    return OK;
}

// displays the current working directory
int print_cwd(FILE* out_file){
    char cwd[PATH_LEN];
    getcwd(cwd, PATH_LEN);
    fprintf(out_file, "%s\n", cwd);
    return OK;
}

// creates a new file if it does not already exist
int create_file(char* path){
    FILE* out = fopen(path, "w");
    if (!out)
        return ERR_1;
    fclose(out);
    return OK;
}

// creates a new directory if not already present
int create_dir(char* path){
    if (mkdir(path, 0777))
        return ERR_1;
    return OK;   
}

int delete_file(char* path){
    if (remove(path))
        return ERR_1;
    return OK;
}

// prints the contents of a file
int print_file(char* path, FILE* out_file){
    FILE* file = fopen(path, "r");
    // ensure the file can be read
    if (!file)     
        return ERR_1;
    // print the file
    char buf[FILE_BUF_LEN];
    while (fgets(buf, FILE_BUF_LEN, file))
        fprintf(out_file, "%s", buf);
    fclose(file);
    return OK;
}

// recursively deletes a directory and children
int delete_recusive(char* path){
    DIR* dir = opendir(path);
    struct dirent* file;
    char tmp[(2 * PATH_LEN) + 1];
    if (!dir)
        return ERR_2;
    while((file = readdir(dir))){
        if (strcmp(file->d_name, ".") && strcmp(file->d_name, "..")){
            sprintf(tmp, "%s/%s", path, file->d_name);
            if (file->d_type == DT_DIR)
                delete_recusive(tmp);
            else
                delete_file(tmp);
        }
    }
    rmdir(path);
    closedir(dir);
    return OK;
}

// deletes a directory, 
int delete_directory(char* path, char** flags, int flag_count){
    if (find_str("-r", flags, flag_count) == -1){
        if (rmdir(path))
            return ERR_1;
    }
    else{
        // check if the user has overriden the warning
        if (find_str("-f", flags, flag_count) == -1){
            printf("Are you sure you would like to delete %s?\nThis will irrevocably delete the contents of the directory. (y/n)\n", path);
            char response = (char) tolower(fgetc(stdin));
            flush_stdin();
            if (response == 'y')
                 return delete_recusive(path);
            else if (response == 'n')
                return ERR_3;
            else
                return ERR_4;
        }
        else 
            return delete_recusive(path);
    }  
    return OK;
}

// echos the proivded arguments
int echo(char** args, int arg_count, FILE* out_file){
    // echo the string to stdout
    for (int i = 0; i < arg_count; i++)
        fprintf(out_file, "%s ", args[i]);
    fprintf(out_file, "\n");
    return OK;
}

// copies the contents of a file to another destination
int copy_file(char* src, char* dest){
    // copy the file
    FILE* in_file = fopen(src, "rb");
    FILE* out_file = fopen(dest, "wb");
    // ensure both files are valid
    if (!in_file || !out_file){
        return ERR_1;
    }
    // get the size of the input file
    fseek(in_file, 0L, SEEK_END);
    size_t file_size = ftell(in_file);
    rewind(in_file);
    // read the file
    unsigned char* buf = (unsigned char*) malloc(sizeof(unsigned char) * file_size);
    fread(buf, file_size, 1, in_file);
    fwrite(buf, file_size, 1, out_file);
    // clean up
    free(buf);
    fclose(in_file);
    fclose(out_file);
    return OK;
}

// moves the contents of one file to another, and deletes the original
int move_file(char* src, char* dest){
    if (copy_file(src, dest))
        return ERR_1;
    if (delete_file(src))
        return ERR_2;
    return OK;
}

// prints the name and parameters of each command
int print_help(FILE* out_file){
    char* commands[COMMAND_COUNT]= {"cd", "ls", "cwd", "create", "created", "del", "rmdir", "cp", "mv", "echo", "echof", "clear", "help", "exit"};
    char* arg_lists[COMMAND_COUNT] = {"(<directory>)", "<directory>", "", "<file name(s)>", "<directory name(s)>", "<file name(s)>", "<directory name(s)>", "<source> <destination>", "<source> <destination>", "<string> (> <file>)", "<file name(s)>", "", "", ""};
    char* flag_lists[COMMAND_COUNT] = {"", "-a", "", "", "", "", "-r, -f", "", "", "", "", "", "", ""};
    fprintf(out_file, "Available Commands:\n\n");
    fprintf(out_file, "\t%-10s %-10s   %s\n", "Name:", "Flags:", "Parameters:");
    for (int i = 0; i < COMMAND_COUNT; i++)
        fprintf(out_file, "\t%-10s [%-10s]   %s\n", commands[i], flag_lists[i], arg_lists[i]);
    fprintf(out_file, "\nArguments in parentheses are optional.\nSee https://github.com/DrewRoss5/ConchShell/blob/main/README.md for more information\n");
    return OK;
}