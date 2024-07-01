#ifndef SHELL_FUNCTIONS_H
#define SHELL_FUNCTIONS_H

#define COMMAND_COUNT 16

enum return_vals{OK=0, ERR_1, ERR_2, ERR_3, ERR_4, FLAG_ERR = 10, INVALID_BIN_ERR = 32512};

int trim_history(FILE** history_file, char* history_path);
int find_str(char* target, char** arr, int size);
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
int exec_bin(char* bin_path, char** argv, int argc, FILE* out_file);
int print_history(FILE* out_file);

#endif