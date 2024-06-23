# ConchShell 🐚
A Linux shell written in C, written as a hobbyist project

# Roadmap/ToDo:
- Make commands ensure that all provided flags are valid
  - Currently, invalid flags are silently ignored
- Allow running external executables
- Add a config file
- Add support for aliases
- Add background tasks
- Add pipelining
- Add support for variables
- Add scripting support

# Installation
To install conch, clone this repo, and from the conchShell directory, run the following commands (requires sudo access):<br>
`chmod install/install.sh +x`<br>`sudo ./install/install.h`

# Usage
Once you have installed conch, simply call `conch` to enter the shell.
### Current commands:
- cd <path>
- ls <path>
- clear
- cwd
  - prints the current working directory, much like `pwd`
- create <path(s)>
  - creates new file(s) if they do not already exist. <br>For example `create foo bar` Will create two files, foo and bar
- created <path(s)>
  - Functions just like mkdir
- del <path(s)>
  - Deletes the provided file(s)
- rmdir \[-r -f] <path(s)>
  - Deletes the provided directory(ies). Must be run with `-r` flag to delete non-empty directories. Run with the `-f` flag to prevent a prompt.
- cp <source> <destination>
- mv <source> <destination>
- echo <argument(s)>
    - functions like the `echo` command in bash, including using ">" to output to files.
- echof <path(s)>
  - prints the contents of the provided files. Much like `cat`.
- help
  - displays a list of all commands and their parameters
- exit
