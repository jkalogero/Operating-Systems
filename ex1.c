#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )

void usage(const char *prog) {
    printf("Usage: %s [--input filname --\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    const char *chosen_file = NULL, *key = NULL;

    // print arguments
    for (int i = 1; i < argc; i++) {
        printf("Argument %d: '%s'\n", i, argv[i]);
    }

    for (int i = 1; i < argc; i++) {
        if (STREQUAL(argv[i], "--input")) {
            if (i == argc-1) {
                // --input was passed as the last argument, error
                usage(argv[0]);
            } else {
                // argv[i] == "--fruit", so argv[i+1] has the choice
                if (strlen(argv[i+1])>0) {
                    chosen_file = argv[i+1];
                } else {
                    usage(argv[0]);
                }
            }
        }
        if (STREQUAL(argv[i], "--key")) {
            if (i == argc-1) {
                // --key was passed as the last argument, error
                usage(argv[0]);
            }
            else{
                if (strlen(argv[i+1])>0) {
                    key = argv[i+1];
                } else {
                    usage(argv[0]);
                }

            }
        }
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
    } else if (pid == 0) {
        printf("CHILD: My pid is %d, my father is %d\n", getpid(), getppid());
    } else {
        printf("PARENT: My pid is %d, my father is %d\n", getpid(), getppid());
        wait(NULL);
    }
    
    return 0;
}