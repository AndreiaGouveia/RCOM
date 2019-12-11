#include "string.h"

#include <stdio.h>
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include<arpa/inet.h>

typedef struct URL {
	char user[256]; // string to user
	char password[256]; // string to password
	char host[256]; // string to host
	char ip[256]; // string to IP
	char path[256]; // string to path
	char filename[256]; // string to filename
	int port; // integer to port
} url;

void initURL(url* url);

int parseURL(url* url, char * argv);

int getIpHost(url* url);