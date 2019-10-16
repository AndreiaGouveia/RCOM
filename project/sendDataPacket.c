#include "sendDataPacket.h"

void getBCC2(unsigned char *data, int sizeData, unsigned char BBC2)
{

    BBC2 = 0x00;
    for (int i = 0; i < sizeData; i++)
    {
        BBC2 = BBC2 ^ data[i];
    }
}

void getSETOfFinalDataPacket(unsigned char set[2], unsigned char *data, int sizeData)
{

    getBCC2(data, sizeData, set[0]);
    set[1] = FLAG;
}

void getSETOfInitialDataPacket(unsigned char set[4])
{

    set[0] = FLAG;
    set[1] = A;
    set[2] = C_SET;
    set[3] = set[1] ^ set[2]; //BCC1
}

void sendInitialDataPacket(int fd, unsigned char set[4])
{
    int res = write(fd, set, 4);

    printf("%d bytes written\n", res);
}

void sendEndDataPacket(int fd,  unsigned char set[2])
{

    int res = write(fd, set, 2);
    printf("%d bytes written\n", res);
}