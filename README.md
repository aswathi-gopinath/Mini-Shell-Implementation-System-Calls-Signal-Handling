# Mini Shell – Linux System Programming

This project implements a custom Linux command-line shell developed using the C programming language and Linux system calls. The mini shell replicates fundamental functionalities of standard Unix shells by accepting user commands, creating processes, executing programs, and managing process termination.

The objective of this project is to understand operating system internals, process management, and low-level system programming concepts used in real Linux environments.

The shell continuously accepts user input, parses commands, creates child processes using fork(), executes commands using exec() family system calls, and waits for execution completion using wait(). Signal handling mechanisms are implemented to safely manage process termination and interrupt handling.

Features:
- Custom interactive Linux shell
- Command execution support
- Process creation using fork()
- Program execution using exec system calls
- Parent-child process synchronization
- Signal handling implementation
- Command parsing and tokenization
- Error handling and validation
- Continuous shell prompt execution

Technologies Used:
- C Programming Language
- Linux System Programming
- POSIX System Calls
- Process Management
- Signal Handling
- GCC Compiler
- Ubuntu Linux Environment

Important System Calls Used:
fork() – Creates child process  
execvp() – Executes external commands  
wait() / waitpid() – Process synchronization  
signal() – Signal handling  
exit() – Process termination  

Project Structure:
minishell/
main.c  
parser.c  
execute.c  
header.h  
README.md  

Compilation:
gcc *.c -o minishell

Execution:
./minishell

Working Principle:
The shell displays a command prompt and waits for user input. The entered command is parsed into tokens and validated. A child process is created using fork(). The child executes the requested program using exec system calls, while the parent process waits for completion. Signals are handled to ensure safe execution and termination of processes.

Learning Outcomes:
- Understanding Linux process lifecycle
- Practical usage of system calls
- Command interpreter design
- Process synchronization techniques
- Signal handling concepts
- Linux operating system internals

Future Improvements:
- Background process execution
- Pipe implementation
- Command history support
- Auto-completion features
- Job control mechanisms

Author:
Aswathi E P  
Embedded Systems Engineer  
Email: aswathigopinath20@gmail.com  
LinkedIn: https://www.linkedin.com/in/aswathi-ep-by0256  
GitHub: https://github.com/aswathi-gopinath  

License:
This project is developed for educational and learning purposes.
