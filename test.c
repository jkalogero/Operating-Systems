#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )

/*void father_handler(int sig){
    if (sig == 10){
        printf("\n[Father process: %d] Will ask current values (SIGUSR1) from all active children processes.\n", getpid());
        for (int i = 0; i < num_of_proc; i++) kill(pid[i], SIGUSR1);
    }
    else if (sig == 12){
        printf("[Process %d] Echo!\n", getpid());
    }
    else if (sig == 15 || sig == 2){
        for (int i = 0; i < num_of_proc; i++){
            printf("[Father process: %d] Will terminate (SIGTERM) child process %d: %d\n", getpid(), i+1, pid[i]);
            kill(pid[i], SIGTERM);
        }
    }
    else if (sig == 31){
        living_children--;
        printf("[Father process: %d] Waiting for %d children.\n", getpid(), living_children);
    }
    else{
        printf("PID = %d....No action for signal %d.\n", getpid(), sig);
    }
} */



int main(int argc, char** argv) {
    int distribution;   //value 0 is for round-robin and value 1 for random
    char *str_distribution;
    int val = 0;
    int rec;
    int num_of_proc;
    int num_of_pipe;
    pid_t pid[100000];

    // no argument is provided
    if (argc < 2){
        printf("Usage: <nChildren> [--random] [--round-robin]\n");     //exw afhsei to %s
        return 0;
    }
    else if (argc == 2){
        num_of_proc = atoi(argv[1]);
        // check it is a positive number
        if (num_of_proc <= 0) printf("Usage: <nChildren> [--random] [--round-robin]\n");
        distribution = 0;

    }
    else if (argc == 3){
        num_of_proc = atoi(argv[1]);
        // check it is a positive number
        if (num_of_proc <= 0) printf("Usage: <nChildren> [--random] [--round-robin]\n");
        if (STREQUAL(argv[2], "--round-robin")) distribution = 0;
        else if (STREQUAL(argv[2], "--random")) distribution = 1;
        else {
            printf("Usage: <nChildren> [--random] [--round-robin]\n");
            return 0;
                }
    }
    else {
        printf("Usage: <nChildren> [--random] [--round-robin]\n");
        return 0;
        }
    num_of_pipe = 2 * num_of_proc;
    int fd[num_of_pipe][num_of_proc];
    for (int i = 0; i < num_of_proc; i++){
        pid[i] = fork();
        if (pid[i] == -1)
        {
            perror("Fork error");
            exit(0);
        }
        else if (pid[i] == 0){
            printf("Child with pid (%d) was created from father with pid (%d)\n",getpid(),getppid());
            close(fd[0][i]);
            close(fd[3][i]);
            //do the work 
            close(fd[1][i]);
            close(fd[2][i]);
            return 0;
        }
        else{
            //father doing staff
            printf("Parent with pid (%d)\n",getpid());
            close(fd[1][i]);
            close(fd[2][i]);
        }
    }
        return 0;

   


}
