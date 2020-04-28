#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <errno.h>


int buffer = 0;
int num_of_proc;
pid_t pid[1000000];
int delay[1000000];


int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}



void child_handler(int sig){  
    if (sig == 10) printf("[Child Process %d: %d] Value: %d!\n", num_of_proc, getpid(), buffer);	
    else if (sig == 12) printf("[Child Process %d] Echo!\n", getpid());
    else if (sig == 15){

        printf("[Child Process %d: %d] Exiting...\n", num_of_proc, getpid());
        exit(0);
    }
    else if (sig == 14){
        printf("[Child process %d: %d] Time Expired! Final Value: %d\n", num_of_proc, getpid(), buffer);
        exit(0);
    }
    else{
        printf("PID = %d....No action for signal %d.\n", getpid(), sig);
    }
}

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
    else{
        printf("PID = %d....No action for signal %d.\n", getpid(), sig);
    }
}



int main(int argc, char** argv) {

    if (argc < 2){
        printf("Error,please provide at least one parameter");
        return 0;
    }

    for (int i = 0; i < argc-1; i++){
        delay[i] = atoi(argv[i+1]);
        if (delay[i] <= 0 ){
            printf("Please provide a positive number for the delays.\n");
            exit(0);
        }
    }

    printf("Maximum execution time of children is set to 50 seconds.\n\n");

    num_of_proc = argc-1;
    int status;
    // start forking...
    printf("[Father process : %d] Was created and will create %d children.\n", getpid(), num_of_proc);
    for (int i = 0; i < argc -1; i++){
        
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
        } 
        else if (pid[i] == 0) {
            // child's code
            alarm(50);
            num_of_proc = i+1;
            printf("[Child Process %d: %d] Was created and will pause!\n", num_of_proc, getpid());
            while(raise(SIGSTOP) != 0){}
            printf("[Child Process %d: %d] Is starting!\n", i+1, getpid());
            // struct for sigaction
	        struct sigaction action;
            action.sa_flags = 0;
            sigemptyset(&action.sa_mask);
	        action.sa_handler=child_handler;
            for (int i = 1; i < 32; i++){
                if (i != 9  &&i != 19) assert (sigaction(i, &action, NULL) == 0);
            }

            while(1){
                buffer++;
                double nano_delay = (double) delay[i];
                msleep(nano_delay*pow(10, 3));
            }
            exit(0);
        }
    }
    for(int i=0; i<num_of_proc; i++){
        int ret, wstatus;
        ret = waitpid(pid[i], &wstatus, WUNTRACED);
        while(ret<=0 || !WIFSTOPPED(wstatus)){ret = waitpid(pid[i], &wstatus, WUNTRACED);}
    }
    
    for (int i=0; i<num_of_proc; i++){
        while(kill(pid[i], SIGCONT) != 0){}
    }
    // struct for sigaction
    struct sigaction action;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    action.sa_handler=father_handler;
    for (int i = 1; i < 32; i++){
        if (i != 9  && i != 19 && i != 17) assert (sigaction(i, &action, NULL) == 0);
    }
    // waiting until all my childen die
    for(int i=0; i<num_of_proc; i++){
        int ret2, wstatus2;
        ret2 = waitpid(pid[i], &wstatus2, WNOHANG);
        while(ret2<=0 || WIFSTOPPED(wstatus2)){ret2 = waitpid(pid[i], &wstatus2, WUNTRACED);}
    }
    printf("All of my children are now dead...\nExiting...\n");
    exit(0);
}
