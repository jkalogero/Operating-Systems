#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )

int num_of_proc;
pid_t pid[1000000];
int pd[1000000][2][2]; //mallon oxi etsi.. theloume omws 2 pipes gia kathe paidi... kapou mporei na thelei kai malloc
pipe(pd); //na tsekaroume kai slack


void father_handler(int sig){
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
}



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
