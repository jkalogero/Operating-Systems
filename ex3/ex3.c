#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>


#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )

// pipe me 4 akra: 0, 2: write, 1,3: read
// 0-1: father-> child,
// 2-3: child -> father

// father (2) 3 --------------> 2 (3) child
// father 0 (1) <-------------- (0) 1 child
#define FATHER_READ     0
#define CHILD_WRITE    1
#define CHILD_READ     2
#define FATHER_WRITE    3


#define DEFAULT "\033[30;1m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[38;1m"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(int argc, char** argv) {
    int distribution;   //value 0 is for round-robin and value 1 for random
    char *str_distribution;
    int val = 0, len;
    int rec;
    int num_of_proc;
    int num_of_pipe;
    pid_t pid[100000];
    int my_pid;

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
    int fd[num_of_proc][4];

    for (int i = 0; i < num_of_proc; i++){
        for(int j = 0; j<2; ++j){
            if (pipe(fd[i] + (2*j)) < 0){
                perror("Failed to assign pipes!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("Parent with pid (%d)\n",getpid());

    for (int i = 0; i < num_of_proc; i++){
        pid[i] = fork();
        if (pid[i] == -1)
        {
            perror("Fork error");
            exit(0);
        }
        else if (pid[i] == 0){
            my_pid = getpid();
            printf("Child with pid (%d) was created from father with pid (%d)\n",my_pid,getppid());
            close(fd[i][FATHER_READ]);
            close(fd[i][FATHER_WRITE]);
            // len = read(fd[i][CHILD_READ], &val , sizeof(val));
            // // check if child managed to read
            // if (len <= 0)
            // {
            //     perror("Child: Failed to read data from pipe");
            //     exit(EXIT_FAILURE);
            // }
            // else{
            //     printf("Child(%d): Just received from father: %d", my_pid, val);
            //     val += 1;
            //     if (write(fd[i][CHILD_WRITE], &val, sizeof(val)) < 0)
            //     {
            //         perror("Child: Failed to write response value");
            //         exit(EXIT_FAILURE);
            //     }
            //     else printf("Child(%d): Sending back to father: %d", my_pid, val);
            // }
            //do the work 
            close(fd[i][CHILD_READ]);
            close(fd[i][CHILD_WRITE]);
            return 0;
        }
        else{
            close(fd[i][CHILD_READ]);
            close(fd[i][CHILD_WRITE]);
            //father doing staff
            // sleep(5);
            // val = 9;
            // if (write(fd[i][FATHER_WRITE], &val, sizeof(val)) != sizeof(val)){
            //     perror("Parent: Failed to send value to child\n");
            //     exit(EXIT_FAILURE);
            // }
            // // read response
            // len = read(fd[i][FATHER_READ], &val, sizeof(val));
            // if (len <= 0){
            //     printf("Parent failed to read..\n");
            //     exit(EXIT_FAILURE);
            // }
            // else printf("Just received from child %d val = %d!\n", i, val);
            wait(NULL);
            close(fd[i][FATHER_READ]);
            close(fd[i][FATHER_WRITE]);
        }
    }
    my_pid = getpid();
    while (1) {
        fd_set inset;
        int maxfd = 0;
        printf("1\n");
        FD_ZERO(&inset);                // we must initialize before each call to select
        printf("2\n");
        FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
        for (int i = 0; i < num_of_proc; i++) {
            FD_SET(fd[i][FATHER_READ], &inset);     // select will check for input from each pipe
            maxfd = MAX(maxfd, fd[i][FATHER_READ]);
            printf("fd[%d][FATHER_READ] = %d\n",i, fd[i][FATHER_READ]);
        }
        printf("3\n");
        printf("STDIN_FILENO = %d\n", STDIN_FILENO);
        // select only considers file descriptors that are smaller than maxfd
        maxfd = MAX(STDIN_FILENO,maxfd) + 1;    //pd[0][num_of_proc-1] is bigger than any other pid in pid array



        printf("maxfd = %d\n", maxfd);

        printf("4\n");
        // wait until any of the input file descriptors are ready to receive
        int ready_fds = select(maxfd, &inset, NULL, NULL, NULL);
        printf("\t\tAAAAAA\n");
        if (ready_fds <= 0) {
            printf("\n\n\n\n\n\nERRORRRRRRRRRR\n\n\n\n\n");
            perror("select");
            continue;                                       // just try again
        }
        printf("5\n");
        // user has typed something, we can read from stdin without blocking
        if (FD_ISSET(STDIN_FILENO, &inset)) {
            char buffer[101];
            int n_read = read(STDIN_FILENO, buffer, 100);   // error checking!
            buffer[n_read] = '\0';                          // why?

            // New-line is also read from the stream, discard it.
            if (n_read > 0 && buffer[n_read-1] == '\n') {
                buffer[n_read-1] = '\0';
            }

            printf(BLUE"Got user input: '%s'"WHITE"\n", buffer);

            if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) {
                // user typed 'exit', kill child and exit properly
                kill(my_pid, SIGTERM);                         // error checking!
                wait(NULL);                                 // error checking!
                exit(0);
            }
        }
        printf("6\n");
        // someone has written bytes to the pipe, we can read without blocking
        for (int i = 0; i < num_of_proc; i++){
            if (FD_ISSET(fd[i][FATHER_READ], &inset)) {
                int val;
                read(fd[i][FATHER_READ], &val, sizeof(int));                 // error checking!

                printf(MAGENTA"Got input from pipe: '%d'"WHITE"\n", val);
            }
        }
    }
    return 0;

   


}
