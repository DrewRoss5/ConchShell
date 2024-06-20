#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include "shell_functions.h"

#define PATH_LEN 256
#define FILE_BUF_LEN 1024
#define _GNU_S

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
        return 1;
    return 0;
}

// runs the LS command
int list_dir(char* path, char** flags, int flag_count){
    // open the file and ensure if it exists
    DIR* dir = opendir(path);
    if (!dir)
        return 1;
    // determine if we're showing hidden directories
    int show_all = (find_str("-a", flags, flag_count) == -1) ? 0 : 1;
    // print the directory contents
    struct dirent* file;
    printf("\t%s:\n", path);
    while ((file = readdir(dir))){
        if (file->d_name[0] != '.' || show_all)
            printf("\t\t%s\n", file->d_name);
    }
    return 0;
}

// displays the current working directory
int print_cwd(){
    char cwd[PATH_LEN];
    getcwd(cwd, PATH_LEN);
    printf("%s\n", cwd);
    return 0;
}

// creates a new file if it does not already exist
int create_file(char* path){
    FILE* out = fopen(path, "w");
    if (!out)
        return 1;
    fclose(out);
    return 0;
}

// creates a new directory if not already present
int create_dir(char* path){
    if (mkdir(path, 0777))
        return 1;
    return 0;   
}

int delete_file(char* path){
    if (remove(path))
        return 1;
    return 0;
}

// prints the contents of a file
int print_file(char* path){
    FILE* file = fopen(path, "r");
    // ensure the file can be read
    if (!file)     
        return 1;
    // print the file
    char buf[FILE_BUF_LEN];
    while (fgets(buf, FILE_BUF_LEN, file))
        printf("%s", buf);
    puts("");   
    fclose(file);
    return 0;
}

// recursively deletes a directory and children
int delete_recusive(char* path){
    DIR* dir = opendir(path);
    struct dirent* file;
    char tmp[(2 * PATH_LEN) + 1];
    if (!dir)
        return 2;
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
    return 0;
}

// deletes a directory, 
int delete_directory(char* path, char** flags, int flag_count){
    if (find_str("-r", flags, flag_count) == -1){
        if (rmdir(path))
            return 1;
    }
    else
        return delete_recusive(path);
    return 0;
}

// echos the proivded arguments
int echo(char** args, int arg_count){
    // determine if the command should echo to a file or stdout
    int insert_pos = find_str(">", args, arg_count);
    if (insert_pos == -1){
        // echo the string to stdout
        for (int i = 0; i < arg_count; i++)
            printf("%s ", args[i]);
        puts("");
    }
    else{
        // echo the string to the provided file
        // ensure a file was provided
        if (insert_pos == (arg_count - 1))
            return 1;
        else{
            // write the file contents
            char* path = args[insert_pos + 1];
            FILE* out = fopen(path, "w");
            if (!out)  
                return 2;
            for (int i = 0; i < insert_pos; i++)
                fprintf(out, "%s ", args[i]);
            fclose(out);
        }
    }
    return 0;
}

// copies the contents of a file to another destination
int copy_file(char* src, char* dest){
    // copy the file
    FILE* in_file = fopen(src, "rb");
    FILE* out_file = fopen(dest, "wb");
    // ensure both files are valid
    if (!in_file || !out_file){
        return 1;
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
    return 0;
}