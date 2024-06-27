#ifndef SHELL_FUNCTIONS_H
#define SHELL_FUNCTIONS_H

#define COMMAND_COUNT 14

enum return_vals{OK=0, ERR_1, ERR_2, ERR_3, ERR_4, FLAG_ERR = 10, FLAG_COUNT_ERR};
typedef struct environment {
    char* cwd;
} Env;

int file_exists(char* dir_name, char* file_name);
int change_dir(char* path);
int list_dir(char* path, char** flags, int flag_count, FILE* out_file);
int print_cwd(FILE* out_file);
int create_file(char* path);
int create_dir(char* path);
int delete_file(char* path);
int delete_directory(char* path, char** flags, int flag_count);
int print_file(char* path, FILE* out_file);
int echo(char** args, int arg_count, FILE* out_file);
int copy_file(char* src, char* dest);
int move_file(char* src, char* dest);
int print_help(FILE* out_file);

#endif
