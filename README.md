# ConchShell 🐚
A Linux shell written in C, written as a hobbyist project

# Roadmap/ToDo:
- Finish implementing basic utilities
- Create an installer
- Allow running external executables
- Add a config file
- Add support for aliases
- Add background tasks
- Add pipelining
- Add support for variables
- Add scripting support

# Usage
Once you have created the `conch` binaries using cmake and make, simply run the executable to enter the shell.
### Current commands:
- cd <path>
- ls <path>
- clear
- cwd
  - prints the current working directory, much like `pwd`
- create <path(s)>
  - creates new file(s) if they do not already exist. <br>For example `create foo bar` Will create two files, foo and bar
- echof <path(s)>
  - prints the contents of the provided files. Much like `cat`.
- exit
