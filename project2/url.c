#include "url.h"

void initURL(url* url) {

    url->port = 21;

}

int parseURL(url* url, char * argv){

// ftp://[<user>:<password>@]<host>/<url-path>

if (sscanf(argv, "ftp://[%[^:]:%[^@]@]%[^/]/%s", url->user, url->password, url->host, url->path) != 4)
{
    printf("sscanf failed\n");
    return 1;
}

return 0;

}