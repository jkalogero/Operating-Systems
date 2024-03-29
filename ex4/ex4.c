#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>    
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include <time.h>


#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )
#define BUF_SIZE 1024


#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"



int SocketSend(int hSocket,char* Rqst,int lenRqst)
{
    int shortRetval = -1;
    shortRetval = write(hSocket, Rqst, lenRqst);
    return shortRetval;
}

int SocketReceive(int hSocket,char* Rsp,int RvcSize)
{
    int shortRetval = -1;
    shortRetval = read(hSocket, Rsp, RvcSize);

    return shortRetval;
}

void slice_str(const char * str, char * buffer, size_t start, size_t end){
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

void eraseZeros(char * light){
    int n;
    if( ( n = strspn(light, "0" ) ) != 0 && light[n] != '\0' ) {       // strspn() - Reference 
        printf("%s \n", &light[n]);         //if no zeros
    } else {
        printf("%s \n", &light[n]);         //after erasing the zeros
    } 
}

int main(int argc, char **argv) {
    int PORT = 8080;
    int sockfd,read_size;
    bool debug_mode = false;
    int n;
    char buffer_read[BUF_SIZE];
    char buffer_write[BUF_SIZE];
    char ip[100];
    char *hostname = "tcp.akolaitis.os.grnetcloud.net";
    struct sockaddr_in server = {0};

    const char * mapper[] = {"boot", "setup", "interval", "button", "motion"};

    int curr_arg = 1;
    while(curr_arg < argc){
        if (STREQUAL(argv[curr_arg], "--host")){
            hostname = argv[curr_arg+1];
            curr_arg +=2;
        }
        else if (STREQUAL(argv[curr_arg], "--port")){
            PORT = atoi(argv[curr_arg+1]);
            curr_arg +=2;
        }
        else if (STREQUAL(argv[curr_arg], "--debug")){
            debug_mode = true;
            curr_arg +=1;
        }
        else{
            printf("Please provide the right arguments\n");
            return 0;
        }
    }
    
    printf("Hostname is %s \n", hostname);
    printf("Current port is %d \n", PORT);

    // construct server struct
    server.sin_family = AF_INET;    //domain
    server.sin_port = htons(PORT);  //port in acceptable form
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf("Error creating the socket.\nExiting.\n");
        return 0;
    }
    struct hostent *he;
	struct in_addr **addr_list;
	int i;
    
    // as arguments set hostname or IP
	if ( (he = gethostbyname(hostname) ) == NULL) {
		//gethostbyname failed
		herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i = 0; addr_list[i] != NULL; i++) {
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );     //convert addrlist to string and copy it to ip
	}
    server.sin_addr.s_addr = inet_addr(ip);         //convert ip to acceptable form
    // deal with warning...
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0){
        printf("Failed to connect to server..");
        return 1;
    }

    //done constructing server struct.. ready to connect

    printf("Connected and waiting for commands!\n");
    printf("Enter the Message: \n");
    while(1){
        char buffer_read[BUF_SIZE] = {0};
        char buffer_write[BUF_SIZE] = {0};
        int n_read = read(STDIN_FILENO, buffer_read, BUF_SIZE-1);
        if (n_read < 0) perror("Error in reading...\n");

        buffer_read[n_read] = '\0';
        if (n_read > 0 && buffer_read[n_read-1] == '\n') {
                buffer_read[n_read-1] = '\0';
            }

        // check user input
        // help
        if (n_read == 5 && strncmp(buffer_read, "help", 6) == 0) {
                printf(MAGENTA"Available commands:\n");
                printf("* 'help'\t\t\t: Print this help message\n");
                printf("* 'exit'\t\t\t: Exit\n");
                printf("* 'get'\t\t\t\t: Retrieve sensor data\n");
                printf("* 'N name surname reason'\t: Ask permission to go out"WHITE"\n");
            }
        // exit
        else if (n_read == 5 && strncmp(buffer_read, "exit", 6) == 0) {
            break;
        }
        // get
        else if (n_read == 4 && strncmp(buffer_read, "get", 4) == 0) {
            if (SocketSend(sockfd, buffer_read, n_read) < 0){
                printf("Error in sending through socket");
                return 0;
            }
            read_size = SocketReceive(sockfd, buffer_write, BUF_SIZE-1);
            if (read_size < 0){
                printf("Error in receiving through socket");
                return 0;
            }
            buffer_write[read_size-1] = '\0';           //change new line to end of string.
            if (debug_mode){
                printf(YELLOW"[DEBUG] sent 'get'\n");
                printf("[DEBUG] read '%s'\n",buffer_write);
            }
            printf(BLUE"-----------------------------------\n");
            printf("Server Response : %s\n\n",buffer_write);

            // event
            char event[1];
            slice_str(buffer_write, event, 0,0);
            printf("Latest event: %s (%s)\n", mapper[atoi(event)], event);


            // temp
            char temp[4];
            slice_str(buffer_write, temp, 6,9);
            double T = atoi(temp)/100.0;
            printf("Temperature is: %.2f\n", T);

            // light
            char light[3];
            slice_str(buffer_write, light, 2,4);
            printf("Light level is: ");
            eraseZeros(light);
            
            // timestamp
            char timestamp[10];
            slice_str(buffer_write, timestamp, 11,20);
            struct tm* timeinfo;
            char time_buffer [80];
            int a = atoi(timestamp);
            long int b = (long int)a;
            timeinfo = localtime(&b);
            strftime(time_buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);
            printf("Timestamp is: %s"WHITE"\n", time_buffer);
        }
        else{
            if (SocketSend(sockfd, buffer_read, n_read) < 0){
                printf("Error in sending through socket");
                return 0;
            }
            read_size = SocketReceive(sockfd, buffer_write, BUF_SIZE-1);
            if (read_size < 0){
                printf("Error in receiving through socket");
                return 0;
            }
            // change '\n' to '\0'
            buffer_write[read_size-1] = '\0';
            if (debug_mode){
                printf(YELLOW"[DEBUG] sent '%s'\n", buffer_read);
                printf("[DEBUG] read '%s'\n", buffer_write);
            }
            printf(CYAN"Send verification code : '%s'"WHITE"\n",buffer_write);}
    }
    close(sockfd);
    return 0;
}
