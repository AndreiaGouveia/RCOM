#include "sendDataPacket.h"

int count_ALARM = 0;
int fd;
unsigned char * set;
int sizeData;

void atende()
{

    printf("atendeu\n");

    if (count_ALARM < 3)
    {

        int res = write(fd, set, sizeData + 6);
        printf("%d bytes written\n", res);

        alarm(3);
    }
    count_ALARM++;
}

void getBCC2(unsigned char *data, int sizeData, unsigned char BBC2)
{

    BBC2 = 0x00;
    for (int i = 0; i < sizeData; i++)
    {
        BBC2 = BBC2 ^ data[i];
    }
}

void getSETDataPacket(unsigned char *data)
{
    set = (unsigned char *)malloc((sizeData + 6) * sizeof(unsigned char));

    set[0] = FLAG;
    set[1] = A;
    set[2] = C_SET;
    set[3] = set[1] ^ set[2]; //BCC1

    for(int i = 1; i <= sizeData; i++)
        set[i + 3] = data[i];

    getBCC2(data, sizeData, set[sizeData + 4]);
    set[sizeData + 5] = FLAG;
}

void LLWRITE(int fileDiscriptor, unsigned char *data, int sizeDataReceived)
{
    fd=fileDiscriptor;
    sizeData = sizeDataReceived;
    unsigned char buf[255];

    getSETDataPacket(data);

    int res = write(fd, set, sizeData + 6);
    printf("%d bytes written\n", res);
    alarm(3);

    int n = 0;
    do
    {

        //Checking if it has send more than 4 times the packet
        if (count_ALARM >= 4)
        {
            printf("Didn't get a response. BYE!\n");
            break;
        }

        res = read(fd, &buf[n], 1);

        //If the read is successful cancels the alarm. If not it continues trying to read
        if (res != -1)
            alarm(0);
        else
            continue;

        printf("%x\n", buf[n]);

        if (n != 0)
        {
            if (buf[n] == FLAG)
                break;
        }
        n++;

    } while (1);
}