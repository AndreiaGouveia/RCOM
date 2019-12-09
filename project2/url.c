#include "url.h"

void initURL(url* url) {

    url->port = 21;

}

int parseURL(url* url, char * argv){

// ftp://[<user>:<password>@]<host>/<url-path>

//Checking if the begin of the url is correct
/*char start[] = "ftp://";

if(strncmp(argv, start, strlen(start)) != 0)
{
    printf("URL doesn't begin with %s\n", start);
    return 1;
}*/

//get the arguments
sscanf(argv, "ftp://[%s:<%s>@]<%s>/<%s>", url->user, url->password, url->host, url->path);

return 0;

}