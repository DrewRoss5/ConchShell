#ifndef COLOR_PRINT
#define COLOR_PRINT

#define COLOR_GREY     "\x1b[30m"
#define COLOR_RED       "\x1b[31m"
#define COLOR_GREEN     "\x1b[32m"
#define COLOR_YELLOW    "\x1b[33m"
#define COLOR_BLUE      "\x1b[34m"
#define COLOR_CYAN      "\x1b[36m"
#define COLOR_WHITE     "\x1b[37m"
#define COLOR_RESET     "\x1b[0m"

void color_printf(char* color, char* format, ...);
void show_error(char* format, ...);

#endif