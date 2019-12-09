#include "string.h"

#include <stdio.h>

typedef struct URL {
	char user; // string to user
	char password; // string to password
	char host; // string to host
	char ip; // string to IP
	char path; // string to path
	char filename; // string to filename
	int port; // integer to port
} url;

void initURL(url* url);

int parseURL(url* url, char * argv);