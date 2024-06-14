typedef struct environment {
    char* cwd;
} Env;

void change_dir(char* path);
void list_dir(char* path);
void print_cwd();
void create_file(char* path);
void create_dir(char* path);
void exit_shell();
