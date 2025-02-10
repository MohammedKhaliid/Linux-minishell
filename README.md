# Linux Mini Shell

## Description
This project is a simple implementation of a Linux shell. It allows users to execute basic shell commands and manage processes. The project aims to provide a deeper understanding of how a shell works and how processes are managed in a Unix-like operating system.

## Features
- Execute basic shell commands (`ls`, `cd`, `pwd`, `echo`, etc.)
- Input/output redirection
- Piping between commands
- Background process execution
- Signal handling (e.g., `Ctrl+C` to terminate a process)

## Installation
To install and run the Linux Mini Shell, follow these steps:

1. Clone the repository:
    ```sh
    git clone git@github.com:MohammedKhaliid/Linux-minishell.git
    ```

2. Navigate to the project directory:
    ```sh
    cd Linux-minishell
    ```

3. Compile the source code:
    ```sh
    make
    ```

## Usage
To start the shell, run the following command:
```sh
./shell
```

Once the shell is running, you can execute various commands such as `ls`, `cd`, `pwd`, `echo`, and more. The shell also supports input/output redirection and piping. For example:
- To list files in a directory:
    ```sh
    ls
    ```
- To change the current directory:
    ```sh
    cd <directory>
    ```
- To print the current working directory:
    ```sh
    pwd
    ```
- To redirect output to a file:
    ```sh
    echo "Hello, World!" > output.txt
    ```
- To pipe the output of one command to another:
    ```sh
    ls | grep "pattern"
    ```
