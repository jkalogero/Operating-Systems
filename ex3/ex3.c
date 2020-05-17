#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )

int num_of_proc;
pid_t pid[1000000];
int fd[1000000][2][2];

int main(int argc, char** argv) {
    int distribution;   //value 0 is for round-robin and value 1 for random
    char *str_distribution;
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

    // if(distribution == 0) printf("round-robin");
    // else printf("random");
    // printf(" method was selected\n");

    // start forking
    for (int i = 0; i < num_of_proc; i++){
        
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
        } 
        else if (pid[i] == 0) {
            // child's code
            num_of_proc = i +1;
            printf("[Child Process %d: %d] Was created with father's pid: %d\n", num_of_proc, getpid(), getppid());
        }
        else{
            // father's code
        }
    }
    return 0;
}
