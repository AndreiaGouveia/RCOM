#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int getInfoFile(unsigned char *initialDataPacket, int sizeInitialDataPacket, char ** nameOfFile, int * sizeOfName, int * sizeOfFile);

int getData(unsigned char * dataPacket, int sizeDataPacket, unsigned char ** fullFile, int beginPosition);
