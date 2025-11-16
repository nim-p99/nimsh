### nimsh
simple shell in C. 

*what is a shell?*
- computer program that provides access to the system on which it runs

*program flow*
1. start shell
2. read user input
3. parse user input - separate command string into program + arguments
4. execute command
5. loop back to 2

To start a new process on unix, we use the fork() system call. This creates a
copy (child) of the current process (parent). The parent process continues to
run while the child process is running. The child then uses the exec() system
call to replace itself with a new program. 

Includes the built-in functions: `cd`, `help`, `exit`.

Limitations:
- any whitespace will be interpreted as a separation of command arguments.

How to run:
- compile (eg gcc)
  ```c
  gcc -o shell shell.c
  ```
- run program
  ```c
  ./shell
  ```


