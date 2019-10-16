#include "sendDataPacket.h"

void getBCC2(unsigned char *data, int sizeData, unsigned char BBC2)
{

    BBC2 = 0x00;
    for (int i = 0; i < sizeData; i++)
    {
        BBC2 = BBC2 ^ data[i];
    }
}

void getSETDataPacket(unsigned char * set, unsigned char *data, int sizeData)
{
    set = (unsigned char *)malloc((sizeData + 6) * sizeof(unsigned char));

    set[0] = FLAG;
    set[1] = A;
    set[2] = C_SET;
    set[3] = set[1] ^ set[2]; //BCC1

    getBCC2(data, sizeData, set[0]);
    set[1] = FLAG;
}

void sendDataPacket(int fd, unsigned char * set)
{
    int res = write(fd, set, 4);

    printf("%d bytes written\n", res);
}