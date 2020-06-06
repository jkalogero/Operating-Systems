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


#define STREQUAL(x, y) ( strncmp((x), (y), strlen(y) ) == 0 )


int SocketSend(int hSocket,char* Rqst,short lenRqst)
{
    int shortRetval = -1;
    /*struct timeval tv;
    tv.tv_sec = 20;   //20 Secs Timeout 
    tv.tv_usec = 0;
    if(setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv)) < 0)
    {
        printf("Time Out\n");
        return -1;
    } */
    shortRetval = send(hSocket, Rqst, lenRqst, 0);
    return shortRetval;
}

int SocketReceive(int hSocket,char* Rsp,short RvcSize)
{
    int shortRetval = -1;
    /*struct timeval tv;
    tv.tv_sec = 20;  //20 Secs Timeout 
    tv.tv_usec = 0;
    if(setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)) < 0)
    {
        printf("Time Out\n");
        return -1;
    }  */
    shortRetval = recv(hSocket, Rsp, RvcSize, 0);
    return shortRetval;
}


int main(int argc, char **argv[]) {
    int PORT = 8080;
    int sockfd,read_size;
    int n;
    char sender[100] = {0};
    char receiver[200] = {0};
    char ip[100];
    char *hostname = "tcp.akolaitis.os.grnetcloud.net";
    struct sockaddr_in server = {0};
    if (argc > 3)
    {
        printf("Too many arguments,no more than 2 allowed!\n");
        return 0;
    }
    if (argc == 3){
        if (STREQUAL(argv[1], "--HOST")){
            hostname = argv[2];
        }
        else if (STREQUAL(argv[1], "--PORT")){
            PORT = atoi(argv[2]);
        }
        else{
            printf("Provide the right argument\n");
            return 0;
        }
    }
    printf("Hostname is %s \n", hostname);
    printf("Current port is %d \n", PORT);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *he;
	struct in_addr **addr_list;
	int i;
		
	if ( (he = gethostbyname(hostname) ) == NULL) {
		//gethostbyname failed
		herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i = 0; addr_list[i] != NULL; i++) {
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
	}
    server.sin_addr.s_addr = inet_addr(ip);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0){
        printf("Failed to connect to server..");
        return 1;
    }
    printf("Connected and waiting for commands!\n");
    printf("Enter the Message: ");
    while(1){  
        gets(sender);
        SocketSend(sockfd, sender, strlen(sender));
        read_size = SocketReceive(sockfd, receiver, 200);
        printf("Server Response : %s\n\n",receiver);
    }
    close(sockfd);
    return 0;
}
