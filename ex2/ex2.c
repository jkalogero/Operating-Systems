#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>





/* File Descriptors for stdin and stdout */
#define FD_STDIN 0
#define FD_STDOUT 1

int main(int argc, char** argv) {
    int write_en;
    if (argc < 2){
        printf("Error,please provide at least one parameter");
        return 0;
    }
    pid_t pid[argc-1];
    int status;
    // start forking...
    for (int i = 0; i < argc -1; i++){
        int delay = atoi(argv[i+1]);
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
        } else if (pid[i] == 0) {
            // child's code
            printf("[Child Process %d: %d] Was created and will pause!\n", i+1, getpid());
            pause();
            printf("[Child Process %d: %d] Is starting!\n", i, getpid());
            // main function of process
            int count = 0;
            while(1){
                count++;
                sleep(delay);
            }
        }
        else{
            // parent's code
            // printf("Will be waiting for child %d\n", pid[i]);
            waitpid(pid[i], &status, WUNTRACED); //kati edw thelei gia na perimenei mexri na pane ola pause...
            printf("Sending SIGCONT to process %d\n", pid[i]);
            kill(pid[i], SIGCONT);


            // for (int i = 0; i < argc-1; i++){
            //     printf("Will be waiting for child %d\n", pid[i]);
            //     waitpid(pid[i], &status, 0);
            //     kill(pid[i], SIGCONT);
            // }

        }
    }
    // for (int i=0; i<argc-1; i++){wait(NULL);}
    return 0;
}
