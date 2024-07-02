cmake_minimum_required(VERSION 3.28)

project(ConchShell)

set(SOURCE_DIR "src/")

add_executable(conch ${SOURCE_DIR}main.c ${SOURCE_DIR}shell_functions.h ${SOURCE_DIR}shell_functions.c ${SOURCE_DIR}color_print.h ${SOURCE_DIR}color_print.c)