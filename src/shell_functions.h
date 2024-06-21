#define COMMAND_COUNT 14

typedef struct environment {
    char* cwd;
} Env;

int file_exists(char* dir_name, char* file_name);
int change_dir(char* path);
int list_dir(char* path, char** flags, int flag_count);
int print_cwd();
int create_file(char* path);
int create_dir(char* path);
int delete_file(char* path);
int delete_directory(char* path, char** flags, int flag_count);
int print_file(char* path);
int echo(char** args, int arg_count);
int copy_file(char* src, char* dest);
int move_file(char* src, char* dest);
int print_help();