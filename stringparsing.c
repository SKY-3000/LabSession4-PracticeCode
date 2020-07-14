#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void sigint_handler(int sig){
    printf("caught sigint\n");
}

void sigtstp_handler(int sig){
    printf("caught sigtstp\n");
}

int main(int argc, char* argv[]){
    
    // signal and handler
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    
    while (1){
        
        //create some space for our strings
        char line[500];
        char argsarray[20][100];
        
        // print prompt
        printf("CS361 >");
        
        // read line from termianl
        fgets(line, 500, stdin);
        
        //break the string up to into words
        char* word = strtok(line, " ");
        int i = 0;
        
        // read from the pipe read end until the pipe is empty
        while(word){
            //copy a word to the arg array
            strcpy(argsarray[i], word);
            if (strncmp(argsarray[i], "exit", 4) == 0){
                exit(0);
            }
            //get next word
            word = strtok(NULL, " \n");
            i = i + 1;
        }
        //argsarray[i] = 0;
       
        // check the condition of the command and parsing the command
        int signal_command = 1;
        char **first_command;
        first_command = malloc(20 * sizeof(char *));
        for (int i=0; i < 20; i++){
            first_command[i] = malloc(500 * sizeof(char));
        }
        char **second_command;
        second_command = malloc(20 * sizeof(char *));
        for (int i=0; i < 20; i++){
            second_command[i] = malloc(500 * sizeof(char));
        }
        
        int k = 0;
        int j = 0;
        while(k < i){
            if (strcmp(argsarray[k], ";") == 0){
                signal_command = 2;
                k++;
            }
            else if (strcmp(argsarray[k], "|") == 0){
                signal_command = 3;
                k++;
            }

            // parsing the command line to different parts
            if (signal_command == 1) {
                strcpy(first_command[k], argsarray[k]);
                k++;
            }
            else {
                strcpy(second_command[j], argsarray[k]);
                j++;
                k++;
            }
            
           // k++;
        }
        first_command[k] = 0;
        second_command[j] = 0;
        
        // command condition 1
        if (signal_command == 1){
            // run the command
            int pid = fork();
            if (pid == 0){
                
                execv(first_command[0], first_command);
                exit(0);
            }
            else {
                int status;
                wait(&status);
                // for print out the status and pid
                printf( "pid:%d status:%d\n", getpid(), WEXITSTATUS(status));
            }
        }
        
        // command conditions 2 which with ;
        else if (signal_command == 2){
            int pid = fork();
            int status;
            
            if (pid == 0) {
                execv(first_command[0], first_command);
                // exit(0);
            }
            else {
                wait(&status);
                // for print out the status and pid
                printf( "pid:%d status:%d\n", getpid(), WEXITSTATUS(status));
            }
            
            int ppid = fork();
            int sstatus;
            if (ppid == 0) {
                execv(second_command[0], second_command);
                // exit(0);
            }
            else {
                wait(&sstatus);
                // for print out the status and pid
                printf( "pid:%d status:%d\n", getpid(), WEXITSTATUS(sstatus));
            }

            
        }

//        // command conditions 3 which with |
//        else {
//            // create pipe
//            int pipefds[2];
//            pid_t pid;
//
//            // create pipe
//            if(pipe(pipefds) == -1){
//                perror("pipe");
//                exit(EXIT_FAILURE);
//            }
//
//            pid = fork();
//            int status;
//
//            if (pid == 0) {
//                //        dup2(pipefds[1], STDOUT_FILENO);
//                // child close the write end
//                close(pipefds[1]);
//                // run the command
//
//                execv(argsarray[0], first_command);
//
//                //after finishing reading, child close the read end
//                close(pipefds[0]);
//
//                //exit(EXIT_SUCCESS);
//            }
//            else {
//                //    dup2(pipefds[0], STDIN_FILENO);
//                //parent close the read end
//                close(pipefds[0]);
//
//                //int status;
//                //waitpid(pid, &status, 0);
//                //wait(&status);
//                // for print out the status and pid
//                printf( "pid:%d status:%d\n", getpid(), WEXITSTATUS(status));
//                //parent write in the pipe write end
//                write(pipefds[1], line, 500);
//                //after finishing writing, parent close the write end
//                close(pipefds[1]);
//                //parent wait for child
//                wait(NULL);
//
//            }
//        }
        
        free(first_command);
        free(second_command);
    } // end for while loop
    return 0;
}
