
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

#define COM0 "/dev/ttyS0"
#define COM1 "/dev/ttyS1"
#define COM2 "/dev/ttyS2"
#define COM3 "/dev/ttyS3"
#define COM4 "/dev/ttyS4"
#define COM5 "/dev/ttyS5"
#define COM6 "/dev/ttyS6"
#define COM7 "/dev/ttyS7"

typedef struct
{
    int fd;
    unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
    unsigned char * frame;          /*Trama*/
    int sizeFrame;
} linkLayer;

linkLayer linkLayerData;

int LLOPEN( int porta, int flag);

int LLCLOSE(int fd);

int LLWRITE(int fd , unsigned char *buffer, int length);

int stuffing(unsigned char *beforeStuffing, int sizeBeforeStuffing);


int readResponse(unsigned char originalFlag, unsigned char cFlag);

void atende();

int readDataPacketSendResponse(unsigned char **dataPacket, int *sizeDataPacket);


int LLREAD(unsigned char **dataPacket, int *sizeDataPacket);


void receivedOK(unsigned char controlBit);

int destuffing(unsigned char * SET, int sizeSET, unsigned char * * afterDestuffing, int * sizeAfterDestuffing);


void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeMessage);


int checkBCC2(unsigned char SET[], int sizeMessage);
