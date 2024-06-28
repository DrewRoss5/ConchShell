# ConchShell 🐚
A Linux shell written in C, written as a hobbyist project

# Roadmap/ToDo:
- Finish implementing running executables
  - ~~Allow executables to be called without explicitly using the "run" command~~
  - ~~Allow passing arguments~~
  - Improve support for intereactive applications
- Add command line history
- Add a config file
- Add support for aliases
- Add background tasks
- Add pipelining
- Add support for variables
- Add scripting support

# Installation
To install conch, clone this repo, and from the conchShell directory, run the following commands (requires sudo access):<br>
`chmod install/install.sh +x`<br>`sudo ./install/install.sh`

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
    - functions like the `echo` command in bash
- echof <path(s)>
  - prints the contents of the provided files. Much like `cat`.
- history
    - prints the contents of the shell history
- run <command(s)>
  - runs a binary and displays its stdout output, includes binaries in any `PATH` directories.
  - NOTES: As of 06/27/2024, this does not completely work with interactive applications.
- help
  - displays a list of all commands and their parameters
- exit
