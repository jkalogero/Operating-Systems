#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


int buffer = 0;
int num_of_proc;
pid_t pid[1000000];

void child_handler(int sig){  
    if (sig == 10) printf("[Child Process %d: %d] Value: %d!\n", num_of_proc, getpid(), buffer);	
    else if (sig == 12) printf("[Child Process %d] Echo!\n", getpid());
    else if (sig == 15){

        // printf("Exiting...\n");
        exit(0);
    }
    else if (sig == 14){
        printf("[Child process %d: %d] Time Expired! Final Value: %d\n", num_of_proc, getpid(), buffer);
        exit(0);
    }
}

void father_handler(int sig){
    printf("\n[Father process: %d] Will ask current values (SIGUSR1) from all active children processes.\n", getpid());
    // sleep(10);
    if (sig == 10){
        for (int i = 0; i < num_of_proc; i++) kill(pid[i], SIGUSR1);
    }
    else if (sig == 12){
        printf("[Process %d] Echo!\n", getpid());
    }
    else if (sig == 15){
        for (int i = 0; i < num_of_proc; i++){
            printf("[Father process: %d] Will terminate (SIGTERM) child process %d: %d\n", getpid(), i, pid[i]);
            kill(pid[i], SIGTERM);
        }
    }
}



int main(int argc, char** argv) {
    
    printf("Maximum execution time of children is set to 50 seconds.\n\n");

    if (argc < 2){
        printf("Error,please provide at least one parameter");
        return 0;
    }
    num_of_proc = argc-1;
    int status;
    // start forking...
    printf("[Father process : %d] Was created and will create %d children.\n", getpid(), num_of_proc);
    for (int i = 0; i < argc -1; i++){
        int delay = atoi(argv[i+1]);
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
        } 
        else if (pid[i] == 0) {
            // child's code
            alarm(50);
            num_of_proc = i+1;
            printf("[Child Process %d: %d] Was created and will pause!\n", num_of_proc, getpid());
            raise(SIGSTOP);
            printf("[Child Process %d: %d] Is starting!\n", i+1, getpid());
            // struct for sigaction
	        struct sigaction action;
            action.sa_flags = 0;
            sigemptyset(&action.sa_mask);
	        action.sa_handler=child_handler;
	        sigaction(SIGUSR1, &action, NULL);
            sigaction(SIGUSR2, &action, NULL);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGALRM, &action, NULL);
            while(1){
                buffer++;
                sleep(delay);
            }
            exit(0);
        }
        else{
            // parent's code
        }
    }
    for(int i=0; i<num_of_proc; i++){
        int ret, wstatus;
        ret = waitpid(pid[i], &wstatus, WUNTRACED);
        while(ret<=0 || !WIFSTOPPED(wstatus)){ret = waitpid(pid[i], &wstatus, WUNTRACED);}
    }
    
    for (int i=0; i<num_of_proc; i++){
        kill(pid[i], SIGCONT);
    }
    // struct for sigaction
    struct sigaction action;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    action.sa_handler=father_handler;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    // while(1){}
    // waiting until all my childen die
    for(int i=0; i<num_of_proc; i++){
       wait(NULL);
    }
    printf("All of my children are now dead...\nExiting...");
    exit(0);
    
    // return 0;
}
