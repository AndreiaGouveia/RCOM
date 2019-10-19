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

#define _SET 0x03
#define _DISC 0x0B
#define _UA 0x07
#define _RR 0x05
#define _REJ 0x01

#define STUFFING 0x7d
#define EXCLUSIVE_OR_STUFFING 0x20


enum ControlField{SET, DISC, UA, RR, REJ}; 

enum WhichControl{Begin, End};

void LLWRITE(int fileDiscriptor, unsigned char *package, int packageSize);
void atende();

unsigned char * getInitialEndDataPacket(FILE * fileToBeSent, char fileName[], enum WhichControl cf,int fileSize);
unsigned char * getSETDataPacket(unsigned char *data, int sizeData);
void receivedOK(int fileDiscriptor, enum ControlField cf, unsigned char controlBit);
unsigned char * readFile(FILE * file, size_t * size, unsigned char *fileName);



int stuffing(unsigned char * beforeStuffing, int sizeBeforeStuffing);

int destuffing(unsigned char * SET, int sizeSET, unsigned char * * afterDestuffing, int * sizeAfterDestuffing);