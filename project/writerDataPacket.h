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
#define C_SET 0x40
#define BCC A ^ C_SET

#define _SET 0x03
#define _DISC 0x0B
#define _UA 0x07
#define _RR 0x05
#define _REJ 0x01

#define STUFFING 0x7d
#define EXCLUSIVE_OR_STUFFING 0x20

typedef struct
{
    int fd;
    unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
    unsigned char * frame;          /*Trama*/
    int sizeFrame;
} linkLayer;

linkLayer linkLayerData;

enum WhichControl { Begin,
                    End };

int LLWRITE(unsigned char *buffer, int length);
void atende();

int getInitialEndDataPacket(FILE *fileToBeSent, char fileName[], enum WhichControl cf, int fileSize, unsigned char **initialSet, int *sizeInitialSet);
unsigned char *getSETDataPacket(unsigned char *data, int sizeData, unsigned char CFlag);
unsigned char *readFile(FILE *file, size_t *size, unsigned char *fileName);

int getFullDataPacket(unsigned char *data, int sizeData, unsigned char **fullData, int *sizefullData, int nSeq);

int stuffing(unsigned char *beforeStuffing, int sizeBeforeStuffing);
