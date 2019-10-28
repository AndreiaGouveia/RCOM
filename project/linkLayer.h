
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include "flags.h"

typedef struct
{
    int fd;
    unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
    unsigned char * frame;          /*Trama*/
    int sizeFrame;
} linkLayer;

linkLayer linkLayerData;

int LLOPEN( char * fd, int flag);

int LLCLOSE(int fd);

int LLWRITE(unsigned char *buffer, int length);

int stuffing(unsigned char *beforeStuffing, int sizeBeforeStuffing);


int readResponse(unsigned char originalFlag, unsigned char cFlag);

void atende();
