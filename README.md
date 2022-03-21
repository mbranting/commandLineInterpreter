# commandLineInterpreter

For program Child2.c

Different files necessary to include to run program 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

To run interactive shell -->
Initiate program by typing ./child2 into terminal
Interactive shell is prompted -->
<<shell>>
Type command on command line
Examples: 
ls -l
touch file
ls -l; cat file; grep foo file2
exit
quit

To run batch file -->
Initiate program by typeing ./child2 batchFile.txt into terminal
Commands in batch file are executed 

