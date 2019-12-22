#include "url.h"

void initURL(url* url) {

    url->port = 21;

}

int parseURL(url* url, char * argv){

// ftp://[<user>:<password>@]<host>/<url-path>

if (sscanf(argv, "ftp://%[^:]:%[^@]@%[^/]/%s", url->user, url->password, url->host, url->path) != 4)
{

    if(sscanf(argv, "ftp://%[^/]/%s", url->host, url->path) != 2)
    { 
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv);
        return 1;
    }

    strcpy(url->user, "anonymous");
    strcpy(url->password, "1");
}

char * ptr = strrchr(url->path, '/');

if(ptr != NULL)
    ptr++;

if(ptr == NULL)
    strcpy(url->filename, url->path);
else
    strcpy(url->filename, ptr);

return 0;

}

int getIpHost(url* url){


    struct hostent *h;

    
    if ((h=gethostbyname(url->host)) == NULL) {  
        herror("gethostbyname");
        return 1;
    }

    /*printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));*/

    char* ip = inet_ntoa(*((struct in_addr *) h->h_addr));
	strcpy(url->ip, ip);

    return 0;

}
