#include <stdio.h>
#include <stdarg.h>
#include "color_print.h"

// prints text in a provided color
void color_printf(char* color, char* format, ...){
    // set the color
    printf("%s", color);
    // get the formatting arguments
    va_list format_args;
    va_start(format_args, format);
    vprintf(format, format_args);
    // reset the color to default
    printf("%s", COLOR_RESET);
}

// prints an error message
void show_error(char* format, ...){
    color_printf(COLOR_RED, "error: ");
    // get the formatting arguments
    va_list format_args;
    va_start(format_args, format);
    vprintf(format, format_args);
}