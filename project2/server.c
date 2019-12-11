#include "server.h"

#include "string.h"

int checkNumCode(char response[], char expected_num_code[]){

    if(strncmp(response, expected_num_code, 3) == 0)
        return 0;

    return 1;

}

int clientTCP(char ip[], int port){

    int	sockfd;
	struct	sockaddr_in server_addr;
	char	buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	return -1;
    	}

	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		return -1;
	}
    
    return sockfd;

}

int connectToServer(server * server, char ip[], int port){

    server->fd_control_socket = clientTCP(ip, port);

    if(server->fd_control_socket == -1)
         return 1;

    char response[1024];

    if (readFromServer(*server, response, sizeof(response)) != 0)
        return 1;

    if(checkNumCode(response, "220") != 0)
    {
        printf(" > Service is not ready!\n");
        return 1;
    }

    printf(" > Service is ready!\n");
    return 0;

}

int loginServer(server server, char username[], char password[]){

    char command[1024];
    char response[1024];

    sprintf(command, "USER %s\n", username);

    if(sendToServer(server, command) != 0)
        return 1;

    printf(" > %s", command);

    if (readFromServer(server, response, sizeof(response)) != 0)
        return 1;

    if(checkNumCode(response, "331") != 0){

        printf("Response was not what was expected: %s\n", response);
        return 0;
    }

    sprintf(command, "PASS %s\n", password);

    if(sendToServer(server, command) != 0)
        return 1;

    printf(" > %s", command);

    if (readFromServer(server, response, sizeof(response)) != 0)
        return 1;

    if(checkNumCode(response, "230") != 0){

        printf("Response was not what was expected: %s\n", response);
        return 0;
    }

}

int sendToServer(server server, char command[]){

    int bytes;

	if (write(server.fd_control_socket, command, strlen(command)) <= 0) {
		perror("sendToServer");
		return 1;
	}

	return 0;
}

int readFromServer(server server, char * response, int size){

/*
    int n = 0;
    int incr = 0;

    while((incr = recv(server.fd_control_socket, &(response[n]), size, 0)) > 0){
        n += incr;
    }

	return 0;*/

    //TODO: I THINK THIS IS NOT 100% CORRECT

    FILE* fp = fdopen(server.fd_control_socket, "r");

	do {

		memset(response, 0, size);
		
        response = fgets(response, size, fp);

        if(response == NULL)
        {
            perror("readFromServer");
            return 1;
        }

        //int incr = recv(server.fd_control_socket, response, size, 0);

		printf(" < %s", response);

	} while (!('1' <= response[0] && response[0] <= '5') || response[3] != ' ');

	return 0;

}