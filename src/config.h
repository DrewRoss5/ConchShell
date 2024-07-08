typedef struct{
    long int history_max;
    char* err_color;
    char* user_color;
    char* path_color; 
} Config;

int parse_config(Config* config, char* config_path);
