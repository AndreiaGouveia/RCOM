#include <stdio.h>

#include "url.h"

int main(int argc, char **argv)
{

    /*if (argc != 2){

		printf("Usage: %s ftp://[username:password]@site/path/to/file\n", argv[0]);
		exit(1);

	}*/

    url URL;
    initURL(&URL);

    char t[] = "ftp://[<user>:<password>@]<host>/<url-path>";
    parseURL(&URL, t);

    printf("PASSOU!\n");


    return 0;

}