
#include <stdio.h>
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <ctype.h>

#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>


typedef struct SERVER
{
    int fd_control_socket; // file descriptor to control socket
     int fd_data_socket; // file descriptor to data socket
} server;


int clientTCP(char ip[], int port);

int connectToServer(server * server, char ip[], int port);

int readFromServer(server server, char * response, int size);

int loginServer(server server, char username[], char password[]);

int psvModeServer(server * server);

int retrServer(server server, char path[]);

int downloadFromServer(server server, char filename[]);

int disconnectToServer(server server);

int sendToServer(server server, char command[]);
