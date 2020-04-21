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
    printf("[Father process : %d] Was created and will create %d children.\n", getpid(), argc-1);
    for (int i = 0; i < argc -1; i++){
        int delay = atoi(argv[i+1]);
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
        } else if (pid[i] == 0) {
            // child's code
            printf("[Child Process %d: %d] Was created and will pause!\n", i+1, getpid());
            // kill(getppid(), SIGCONT);
            raise(SIGSTOP);
            printf("[Child Process %d: %d] Is starting!\n", i+1, getpid());
            // main function of process
            int count = 0;
            while(1){
                count++;
                sleep(delay);
            }
            exit(0);
        }
        else{
            // parent's code


        }
    }
    for(int i=0; i<argc-1; i++){
        int ret, wstatus;
        ret = waitpid(pid[i], &wstatus, WUNTRACED);
        while(ret<=0 || !WIFSTOPPED(wstatus)){ret = waitpid(pid[i], &wstatus, WUNTRACED);}

    }
    for (int i=0; i<argc-1;i++){
        kill(pid[i], SIGCONT);
        // wait(NULL);
    }
        while(1){}
    
    return 0;
}
