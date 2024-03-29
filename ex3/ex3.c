#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )



#define FATHER_READ     0
#define CHILD_WRITE    1
#define CHILD_READ     2
#define FATHER_WRITE    3


#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int num_of_proc;
pid_t pid[100000];

void father_handler(int sig){
    if (sig == 15){
        for (int i = 0; i < num_of_proc; i++){
            printf(YELLOW"[Father process: %d] Will terminate (SIGTERM) child process %d: %d"WHITE"\n", getpid(), i+1, pid[i]);
            kill(pid[i], SIGTERM);
        }
    }
    else{
        printf("PID = %d....No action for signal %d.\n", getpid(), sig);
    }
}

bool isNumber(char number[])
{
    int i = 0;
    for (; number[i] != 0; i++){
        if (!isdigit(number[i]))
            return false;
    }
    return true;
}


int main(int argc, char** argv) {
    int distribution;   //value 0 is for round-robin and value 1 for random
    int val = 0, len;
    int num_of_pipe;
    int my_pid;
    int index = 0;

    // no argument is provided
    if (argc < 2){
        printf("Usage: <nChildren> [--random] [--round-robin]\n");     //exw afhsei to %s
        return 0;
    }
    else if (argc == 2){
        num_of_proc = atoi(argv[1]);
        // check it is a positive number
        if (num_of_proc <= 0) {
            printf("Usage: <nChildren> [--random] [--round-robin]\n");
            return 0;}
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
            printf(YELLOW"Child with pid (%d) was created from father with pid (%d)"WHITE"\n",my_pid,getppid());
            close(fd[i][FATHER_READ]);
            close(fd[i][FATHER_WRITE]);
            int val;
            while(1){
                len = read(fd[i][CHILD_READ], &val , sizeof(val));
                // check if child managed to read
                if (len <= 0)
                {
                    perror("Child: Failed to read data from pipe");
                    exit(EXIT_FAILURE);
                }
                else{
                    printf(BLUE"[Child "MAGENTA"%d"BLUE"] [%d] Child received "CYAN"%d"BLUE"!"WHITE"\n", i+1, my_pid, val);
                    val += 1;
                    sleep(5);
                    if (write(fd[i][CHILD_WRITE], &val, sizeof(val)) < 0)
                    {
                        perror("Child: Failed to write response value");
                        exit(EXIT_FAILURE);
                    }
                    else printf(RED"[Child "MAGENTA"%d"RED"] [%d] Child finished hard work, writing back "CYAN"%d"WHITE"\n", i+1 , my_pid, val);
                }

            }
            close(fd[i][CHILD_READ]);
            close(fd[i][CHILD_WRITE]);
            return 0;
        }
        else{
            close(fd[i][CHILD_READ]);
            close(fd[i][CHILD_WRITE]);
        }
    }
    my_pid = getpid();

    // struct for sigaction
    struct sigaction action;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    action.sa_handler=father_handler;
    assert (sigaction(15, &action, NULL) == 0);


    while (1) {
        fd_set inset;
        int maxfd = 0;
        FD_ZERO(&inset);                // we must initialize before each call to select
        FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
        
        
        for (int i = 0; i < num_of_proc; i++) {
            FD_SET(fd[i][FATHER_READ], &inset);     // select will check for input from each pipe
            maxfd = MAX(maxfd, fd[i][FATHER_READ]);
        }


        // select only considers file descriptors that are smaller than maxfd
        maxfd = MAX(STDIN_FILENO,maxfd) + 1;


        // wait until any of the input file descriptors are ready to receive
        int ready_fds = select(maxfd, &inset, NULL, NULL, NULL);
        if (ready_fds <= 0) {
            perror("select");
            continue;
        }
        // user has typed something, we can read from stdin without blocking
        if (FD_ISSET(STDIN_FILENO, &inset)) {
            printf("GOT USER INPUT\n");
            char buffer[101];
            int n_read = read(STDIN_FILENO, buffer, 100);
            buffer[n_read] = '\0';

            // New-line is also read from the stream, discard it.
            if (n_read > 0 && buffer[n_read-1] == '\n') {
                buffer[n_read-1] = '\0';
            }

            // help
            if (n_read == 5 && strncmp(buffer, "help", 4) == 0) {
                printf(MAGENTA"Type a number to send job to a child!"WHITE"\n");
            }

            // exit
            else if (n_read == 5 && strncmp(buffer, "exit", 4) == 0) {
                // user typed 'exit', kill child and exit properly
                for (int i  = 0; i < num_of_proc; i++){
                    close(fd[i][FATHER_READ]);
                    close(fd[i][FATHER_WRITE]);
                }
                kill(my_pid, SIGTERM);
                wait(NULL);
                printf(RED"All of my children are now dead...\nExiting."WHITE"\n");

                exit(0);
            }

            // user entered integer
            else if (isNumber(buffer)){
                val = atoi (buffer);
                int choose_pipe;
                if (distribution == 0){
                    choose_pipe = index;
                    if (index == num_of_proc-1) index = 0;
                    else index++;
                }
                else if (distribution == 1){
                    choose_pipe = rand()%num_of_proc;
                }
                printf(CYAN"[Parent] Assigned %d to child %d"WHITE"\n", val, choose_pipe+1);
                if (write(fd[choose_pipe][FATHER_WRITE], &val, sizeof(val)) != sizeof(val)){
                    perror("Parent: Failed to send value to child\n");
                    exit(EXIT_FAILURE);
                }
            
            }
            else{
                printf(MAGENTA"Type a number to send job to a child!"WHITE"\n");
            }
        }
        // someone has written bytes to the pipe, we can read without blocking
        for (int i = 0; i < num_of_proc; i++){
            if (FD_ISSET(fd[i][FATHER_READ], &inset)) {
                int val;
                int len_read = read(fd[i][FATHER_READ], &val, sizeof(int));
                if (len_read <= 0){
                    perror("Parent failed to read...\n");
                }
                else printf(GREEN"[Parent] received result from child %d --> "CYAN"%d"WHITE"\n", i+1, val);
            }
        }
    }
    return 0;

   


}
