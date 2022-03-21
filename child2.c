// Program: Command line Interpreter / Shell
//   Shell should provide interactive as well as batch commands (commands sent into the program)
// Interactive Shell
//   x define your own custom prompt
//   x create child process: use execv() and fork()
//   x execute the command entered by user
//   x when multiple commands are entered, separated by semicolons, they are executed simultaneously
//     (use wait() and waitpid())
//   x return to prompt
// Batch Shell Mode
//   x user creates a batch file with a list of commands
//   x start shell by providing the name of the batch file as argument
//   x when in batch mode, do not display the prompt until execution of ALL commands is complete
//     (use wait() and waitpid())
//   x Display (echo) each line in batch before executing
//   x Define/implement following two functions:
//      a. End execution
//      b. Exit the shell
//   - Define a combination of keys for the above functions (e.g. CTRL-X to exit)
//   x The batch file can contain multiple commands seperate by semicolons

//Website used to structure code: https://brennan.io/2015/01/16/write-a-shell-in-c/
//McKenna Branting
//This is my own work

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

//forward declaring functions (defined below)
char *readLine();
char **parseLine(char *line, const char *delim);
int processLine(char* line);
int executeCommand(char **args);
void processBatchFile(const char* filename);

// my custom shell prompt
#define prompt "<<shell>>"

int main(int argc, char *argv[])
{
    char *line = NULL;

    // Batch mode: if they passed in a file as an argument
    if (argc > 1) {
        printf("Processing Batch File: %s \n", argv[1]);
        processBatchFile(argv[1]);
    }

    // Interactive Shell Mode
    while (1)
    {
        int status = 1;

        // print out our custom prompt, and interactively read the commands
        printf("%s", prompt);
        line = readLine();
        if (strcmp(line,"")) {
            //printf("Line Read: %s\n", line); // debugging
            // Process this line (may be a single command, or multiple commands seperated by ';')
            status = processLine(line);
        }

        // Break out of the shell (while loop) if we received our exit command
        if (status != 1)
        {
            break;
        }
    }

    // free resources
    if (line) {
        free(line);
        line = NULL;
    }
}

// read a line from the standard input (removes any newline characters at the end)
char* readLine() {
    //two parameters for getline function 
    char* line = NULL;
    size_t ignore = 0;
    ssize_t read;

    if ((read = getline(&line, &ignore, stdin)) == -1) {
        perror("Error Reading Line");
        exit(EXIT_FAILURE);
    }

    // remove '\n'
    if (line[read - 1] == '\n'){
        line[read - 1] = '\0';
    }

    return line;
}

// split the line into multiple strings, specified by the delimiter 
char** parseLine(char* line, const char* delim) {
    int MAX_SIZE = 1024, position = 0;
    char** tokens = (char**)malloc(MAX_SIZE * sizeof(char*));
    char* token;

    token = strtok(line, delim);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        token = strtok(NULL, delim);
    }

    tokens[position] = NULL;
    return tokens;
}

// process a line which might be multiple commands (seperated by ';') then execute them
int processLine(char* line) {
    
    // return a status of 1
    int status = 1;

    //split up and print commands using semicolon delim
    char **commands = parseLine(line, ";");
    char *command = commands[0];
    int i = 0;
    
    while (command != NULL) {
        //split each command into its arguments
        //printf("command[%d]: %s\n", i, command); // debugging
        char **args = parseLine(command, " ");

        //execute this command
        status = executeCommand(args);
        free(args);

        // get the next command
        command = commands[++i];
    }

    // free resources    
    free(commands);

    return status;
}

// execute the command (fork, then exec) and wait till finished
int executeCommand(char** args) {
    int pid, wpid;
    int status;

    // check if it was an exit
    if ((strcmp(args[0], "quit") == 0) || (strcmp(args[0], "exit") == 0))
    {
        return 5;
    }

    pid = fork();

    if (pid == 0) {
        // child process, execute command
        if (execvp(args[0], args) == -1) {
            // Error executing this process
            perror("Error executing command");
        }
        // if execvp returns, an error occurred (or they quit the shell)
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Error forking");
    } else {
        // Parent Process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// process the batch file, line by line
void processBatchFile(const char* filename) {

    char* line;
    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(EXIT_FAILURE);
    }

    // process each line
    int i = 0;
    size_t len = 0;
    while ((getline(&line, &len, fptr)) != -1)
    {

        // echo the line read to the screen
        printf("Line[%d]: %s", i++, line);

        // remove '\n'
        size_t len = strlen(line);
        if (line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        // process the line
        if (strcmp(line, ""))
        {
            int status = processLine(line);
        }
    }

    // free resources
    fclose(fptr);
    if (line) {
        free(line);
        line = NULL;
    }
}

