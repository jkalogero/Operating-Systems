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
    printf("argc-1 = %d\n", argc-1);
    // start forking...
    for (int i = 0; i < argc -1; i++){
        int d = atoi(argv[i+1]);
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
        } else if (pid == 0) {
            // child's code
            printf("Im child no %d , my pid is %d, my parent's pid = %d\n",i+1, getpid(), getppid());
            pause();
        }
        else{
            // printf("My pid is %d, my parent's pid = %d\n", getpid(), getppid());
            // parent's code
        }
    }
    // for (int i=0; i<argc-1; i++){wait(NULL);}
    return 0;
}
