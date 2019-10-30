#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "flags.h"
#include "linkLayerWriter.h"

enum WhichControl { Begin,
                    End};

int getControlDataPacket(FILE *fileToBeSent, char fileName[], enum WhichControl cf, int fileSize, unsigned char **initialSet, int *sizeInitialSet);
unsigned char *getSETDataPacket(unsigned char *data, int sizeData, unsigned char CFlag);
unsigned char *readFile(FILE *file, size_t *size, unsigned char *fileName);

int getFullDataPacket(unsigned char *data, int sizeData, unsigned char **fullData, int *sizefullData, int nSeq);

void sendFileData(int fd, int fileSize, unsigned char * fullData);

void sendControlDataPacket(int fd, enum WhichControl cf, unsigned char * fileName, FILE * file, int fileSize);

void sendUA(int fd);
