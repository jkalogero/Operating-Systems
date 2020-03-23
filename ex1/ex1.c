#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )

/* File Descriptors for stdin and stdout */
#define FD_STDIN 0
#define FD_STDOUT 1

/* Arbitrary buffer size */
#define BUFFER_SIZE 128

/* User read-write, group read, others read */
#define PERMS 0644

/* Enum for encryption mode */
typedef enum {
    ENCRYPT,
    DECRYPT
} encrypt_mode;

void usage(const char *prog) {
    printf("Usage: %s [--input filname --\n", prog);
    exit(EXIT_FAILURE);
}

char caesar(unsigned char ch, encrypt_mode mode, int key)
{
    if (ch >= 'a' && ch <= 'z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'a') ch += 26;
        }
        return ch;
    }

    if (ch >= 'A' && ch <= 'Z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'Z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'A') ch += 26;
        }
        return ch;
    }

    return ch;
}

int main(int argc, char** argv) {
    const char *chosen_file = NULL, *key = NULL;
    int write_en;
    if (argc != 5){
        printf("Error,wrong number of arguments given");
        return 0;
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
        // child's code
        printf("CHILD: My pid is %d, my father is %d\n", getpid(), getppid());
        int n_read, n_write;
        char buffer[BUFFER_SIZE], encrypted[BUFFER_SIZE];

        int fd_in = open(chosen_file, O_RDONLY);
        if (fd_in == -1) {
            perror("open");
            exit(-1);
        }
        do {
            // Read from file
            n_read = read(fd_in, buffer, BUFFER_SIZE);
            if (n_read == -1) {
                perror("read");
                exit(-1);
            }

        } while (n_read > 0); // (why?)

        // Close input file
        close(fd_in);

        // encryption
        for (int i=0; i< BUFFER_SIZE; i++){
            encrypted[i] = caesar(buffer[i], ENCRYPT, atoi(key));
        }

        // open encrypted.txt file
        int fd_in_enc = open("encrypted.txt", O_WRONLY);
        if (fd_in_enc == -1){
            perror("open");
            exit(-1);
        }
        else{
            // write in encrypted.txt file
            write_en = write(fd_in_enc, encrypted, BUFFER_SIZE);
            if (write_en < BUFFER_SIZE){
                perror("write");
                exit(-1);
            }
            // close encrypted.txt
            close(fd_in_enc);
            exit(0);
        }
    }
    else {
        // PARENT'S CODE
        printf("PARENT: My pid is %d, my father is %d\n", getpid(), getppid());
        wait(NULL);
        pid_t pid2 = fork();
        if (pid2 == -1) {
            perror("fork");
        } else if (pid2 == 0) {
            // CHILD'S CODE
            printf("child2: My pid is %d, my father is %d\n", getpid(), getppid());
            char text[BUFFER_SIZE], decrypted[BUFFER_SIZE];
            int fd_in_2 = open("encrypted.txt", O_RDONLY);
            if (fd_in_2 == -1){
                perror("open");
                exit(-1);
            }
            else{
                int n_read_2, n_write_2;
                    n_read_2 = read(fd_in_2, text, BUFFER_SIZE);
                    if (n_read_2 == -1) {
                        perror("read");
                        exit(-1);
                    }

                // encryption
                for (int i=0; i< BUFFER_SIZE; i++){
                    decrypted[i] = caesar(text[i], DECRYPT, atoi(key));
                }

                // do {
                    // Write at most n_read bytes (why?), returns number of bytes written
                    n_write_2 = write(FD_STDOUT, decrypted, n_read_2);
                    if (n_write_2 < n_read_2) {
                        perror("write");
                        exit(-1);
                    }
                // } while (n_read_2 > 0); // (why?)
                exit(0);
            }
        }
         else {
            printf("PARENT: My pid is %d, my father is %d\n", getpid(), getppid());
            wait(NULL);
            exit(0);
        }
    }
    
    return 0;
}