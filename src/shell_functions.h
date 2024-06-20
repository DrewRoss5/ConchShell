typedef struct environment {
    char* cwd;
} Env;

int file_exists(char* dir_name, char* file_name);
void change_dir(char* path);
void list_dir(char* path, char** flags, int flag_count);
void print_cwd();
void create_file(char* path);
void create_dir(char* path);
void delete_file(char* path);
void delete_directory(char* path, char** flags, int flag_count);
void print_file(char* path);
void echo(char** args, int arg_count);
void copy_file(char* src, char* dest);