#include <stdio.h>

#include "url.h"
#include "server.h"

int main(int argc, char **argv)
{

    if (argc != 2){

		printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
		exit(1);

	}

    url URL;
    initURL(&URL);

    //1. Interpret command line
    if (parseURL(&URL, argv[1]) != 0)
        return 1;

    printf("Command Line Arguments: %s %s %s %s\n", URL.user, URL.password, URL.host, URL.path);

    //2. Get Ip Address (similar to getip.c)
    if(getIpHost(&URL) != 0)
        return 1;

    printf("Ip Address of host: %s\n", URL.ip);


    server server;

    //3. Create TCP Socket (similar to clientTCP.c): using port 21 and IP address recovered in 2 and check if connection went through
    if (connectToServer(&server, URL.ip, URL.port) != 0)
        return 1;
        

    //4. Login User    
    if(loginServer(server, URL.user, URL.password) != 0)
        return 1;

    //5. Entering Passive Mode and Creating a TCP Socket for data
    if(psvModeServer(&server) != 0)
        return 1;

    //6. Sending 
    if(retrServer(server, URL.path) != 0)
        return 1;

    if(downloadFromServer(server, URL.filename) != 0)
        return 1;

    return 0;

}