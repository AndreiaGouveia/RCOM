#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#define FLAG 0x7e
#define A 0x03
#define C_SET 0x30
#define BCC A ^ C_SET

#define STUFFING 0x7d
#define EXCLUSIVE_OR_STUFFING 0x20

#define _SET 0x03
#define _DISC 0x0B
#define _UA 0x07
#define _RR 0x05
#define _REJ 0x01

#define FALSE 0
#define TRUE 1


enum ControlField{SET, DISC, UA, RR, REJ}; 

int destuffing(unsigned char * SET, int sizeSET, unsigned char * * afterDestuffing, int * sizeAfterDestuffing);


void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeMessage);


int checkBCC2(unsigned char SET[], int sizeMessage);


void receivedOK(int fileDiscriptor, enum ControlField cf, unsigned char controlBit);

int LLREAD(int fd, unsigned char * dataPacket, int *sizeDataPacket);

int getSizeFile(unsigned char *initialDataPacket, int sizeInitialDataPacket, unsigned char ** nameOfFile, int * sizeOfName, int * sizeOfFile);