#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "config.h"
#include "color_print.h"
#include "shell_functions.h"

#define LINE_SIZE 1024

enum OPTIONS {HISTORY_MAX = 0, ERR_COLOR, USR_COLOR, PATH_COLOR};

// sets the pointed to string to a color code based on
int parse_color(char* color_name, char** color_code){
    int color_count = 7;
    char* color_codes[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_GREY, COLOR_WHITE, COLOR_RESET};
    char* color_names[] = {"red", "green", "blue", "yellow", "grey", "white", "default"};
    for (int i = 0; i < color_count; i++){
        if (strcmp(color_names[i], color_name) == 0){
            *color_code = color_codes[i];
            return OK;
        }
    }
    return ERR_1;
}

// takes an option name and returns it's corresponding number, or 4 if it's invalid
int parse_option(char* option){
    int option_count = 4;
    char* options[] = {"max_history_lines", "error_color", "user_color", "path_color"};
    for (int i = 0; i < option_count; i++){
        if (strcmp(option, options[i]) == 0)
            return i;
    }
    return 4;
}

// finds the "=" delimeter in the string and returns it's position, or -1 if it cannot be found
int find_delim(char* str, char delim){
    int delim_pos = -1;
    size_t len = strlen(str);
    for (int i = 0; i < len; i++){
        if (str[i] == delim){
            delim_pos = i;
            break;
        }
    }
    return delim_pos;
}

// reads the configuration file, and returns the 
int parse_config(Config* conf, char* config_path){
    // set defaults
    conf->history_max = 1024;
    conf->err_color = COLOR_RED;
    conf->user_color = COLOR_CYAN;
    conf->path_color = COLOR_GREEN;
    // read the file
    FILE* config_file = fopen(config_path, "r");
    if (!config_file)
        return ERR_1;
    char buf[LINE_SIZE];
    char* option_str;
    char* val;
    int delim_pos;
    int option;
    int line_no = 1;
    while (fgets(buf, LINE_SIZE, config_file)){
        delim_pos = find_delim(buf, '=');
        if (delim_pos == -1){
            errno = line_no;
            return ERR_2;
        }
        option_str = buf;
        option_str[delim_pos] = '\0';
        val = buf + (delim_pos + 1);
        // strip the new line from val
        val[strlen(val) - 1] = '\0';
        // set the appropriate option
        char* color;
        switch (parse_option(option_str)){
        case HISTORY_MAX:
            int history_max = atoi(val);
            if (val <= 0){
                errno = line_no;
                return ERR_3;
            }
            conf->history_max = history_max;
            break;
        case ERR_COLOR:
            if (parse_color(val, &color)){
                errno = line_no;
                return ERR_3;
            }
            conf->err_color = color;
            break;
        case USR_COLOR:
            if (parse_color(val, &color)){
                errno = line_no;
                return ERR_3;
            }
            conf->user_color = color;
            break;
        case PATH_COLOR:
            if (parse_color(val, &color)){
                errno = line_no;
                return ERR_3;
            }
            conf->path_color = color;
            break;
        
        default:
            errno = line_no;
            return ERR_2;
            break;
        }
        line_no++;
    }
    return OK;

}