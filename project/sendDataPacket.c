#include "sendDataPacket.h"

int count_ALARM = 0;
int fd;
unsigned char *set;
int sizeSet;

void atende()
{

    printf("atendeu\n");

    if (count_ALARM < 3)
    {

        int res = write(fd, set, sizeSet);
        printf("%d bytes written\n", res);

        alarm(3);
    }
    count_ALARM++;
}

void getBCC2(unsigned char *data, int sizeData, unsigned char *BBC2)
{

    *BBC2 = 0x00;
    for (int i = 0; i < sizeData; i++)
    {
        *BBC2 = *BBC2 ^ data[i];
    }
}

unsigned char *getSETDataPacket(unsigned char *data, int sizeData)
{
    unsigned char * setBefore = (unsigned char *)malloc((sizeData + 6) * sizeof(unsigned char));

    setBefore[0] = FLAG;
    setBefore[1] = A;
    setBefore[2] = C_SET;
    setBefore[3] = setBefore[1] ^ setBefore[2]; //BCC1

    for (int i = 1; i <= sizeData; i++)
    {
        setBefore[i + 3] = data[i - 1];
    }

    getBCC2(data, sizeData, &set[sizeData + 4]);
    setBefore[sizeData + 5] = FLAG;

    return setBefore;
}

void LLWRITE(int fileDiscriptor, unsigned char *package, int packageSize)
{
    fd = fileDiscriptor;
    unsigned char buf[255];

    stuffing(package, packageSize, set, &sizeSet);

    int res = write(fd, set, sizeSet);
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

        printf("%0x\n", buf[n]);

        if (n != 0)
        {
            if (buf[n] == FLAG)
                break;
        }
        n++;

    } while (1);
}

void receivedOK(int fd, enum ControlField cf, unsigned char controlBit)
{

    unsigned char sendDataPacket[5];

    sendDataPacket[0] = FLAG;
    sendDataPacket[1] = A;

    switch (cf)
    {
    case SET:
        sendDataPacket[2] = _SET;
        break;

    case DISC:
        sendDataPacket[2] = _DISC;
        break;

    case UA:
        sendDataPacket[2] = _UA;
        break;

    case RR:
        sendDataPacket[2] = _RR;

        sendDataPacket[2] |= controlBit << 1;
        break;

    case REJ:
        sendDataPacket[2] = _REJ;

        sendDataPacket[2] |= controlBit << 1;
        break;
    }

    sendDataPacket[3] = sendDataPacket[1] ^ sendDataPacket[2];
    sendDataPacket[4] = FLAG;

    write(fd, sendDataPacket, 5);
}

long int findSize(FILE *fp)
{

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    long int res = ftell(fp);

    // closing the file
    fclose(fp);

    return res;
}

unsigned char *getInitialEndDataPacket(FILE *fileToBeSent, char fileName[], enum WhichControl cf, int fileSize)
{

    //alocating the space for the initialSet
    int initialSize = 6 * sizeof(unsigned char) + strlen(fileName);
    unsigned char *initialSet = (unsigned char *)malloc(initialSize);

    if (cf == Begin)
    {
        initialSet[0] = 0x02;
    }
    else
        initialSet[0] = 0x03;

    //TYPE - LENGTH - VALUE
    
    //Size of file
    initialSet[1] = 0x00;
    initialSet[2] = 0x01;
    initialSet[3] = fileSize;

    //Name of file
    initialSet[4] = 0x01;
    initialSet[5] = strlen(fileName);

    for (int i = 0; i < strlen(fileName); i++)
    {
        initialSet[i + 6] = fileName[i];
    }

    return initialSet;
}

unsigned char *readFile(FILE *file, size_t *size, unsigned char *fileName)
{
    struct stat fileInfo;

    stat((char *)fileName, &fileInfo); //gets file info

    (*size) = fileInfo.st_size; //gets size of file

    unsigned char *fullData = (unsigned char *)malloc(*size);

    fread(fullData, sizeof(unsigned char), *size, file); //reads file and store it in fullData

    return fullData;
}

int stuffing(unsigned char * SET, int sizeSET, unsigned char * afterStuffing, int * sizeAfterStuffing){

    //allocating necessary space
    afterStuffing = (unsigned char *) malloc(sizeof(unsigned char) * sizeSET);

    //INITIAl FLAG
    afterStuffing[0] = SET[0];

    //current position on the afterStuffing array
    int currentPositionOfStuffing = 1;

    //Size of the afterStuffing array
    int newSize=sizeSET;

    for(int i = 1; i < sizeSET-1; i++, currentPositionOfStuffing++){

        if(SET[i] == FLAG || SET[i] == STUFFING)
        {
            newSize += 2;
            afterStuffing=realloc(afterStuffing, newSize);

            afterStuffing[currentPositionOfStuffing]=STUFFING;
            afterStuffing[currentPositionOfStuffing + 1] = SET[i] ^ EXCLUSIVE_OR_STUFFING;
            currentPositionOfStuffing++;

        }else
        {
            afterStuffing[currentPositionOfStuffing]=SET[i];
        }
        
    }

    //END FLAG
    afterStuffing[newSize-1] = SET[sizeSET - 1];

    printf("==================================STUFFING==================================");
    for(int i = 0; i < newSize; i++){
        printf("%0x\n", afterStuffing[i]);
    }

    printf("==================================BEFORE STUFFING===========================");
    for(int i = 0; i < sizeSET; i++){
        printf("%0x\n", SET[i]);
    }

    

    *sizeAfterStuffing=newSize;

    return 0;
}

int destuffing(unsigned char * SET, int sizeSET, unsigned char * afterDestuffing, int * sizeAfterDestuffing){

    //allocating necessary space
    afterDestuffing = (unsigned char *) malloc(sizeof(unsigned char) * sizeSET);

    //INITIAl FLAGs
    afterDestuffing[0] = SET[0];

    //current position on the afterStuffing array
    int currentPositionOfDestuffing = 1;

    //Size of the afterStuffing array
    int newSize=sizeSET;

    for(int i = 1; i < sizeSET-1; i++, currentPositionOfDestuffing++){

        if(SET[i] == STUFFING)
        {
            newSize -= 2;
            afterDestuffing=realloc(afterDestuffing, newSize);

            afterDestuffing[currentPositionOfDestuffing] = SET[i+1] ^ EXCLUSIVE_OR_STUFFING;

            //Advance the second argument of XOR
            i++;

        }else
        {
            afterDestuffing[currentPositionOfDestuffing]=SET[i];
        }
        
    }

    
    //END FLAG
    afterDestuffing[newSize-1] = SET[sizeSET - 1];

    *sizeAfterDestuffing=newSize;

    return 0;
}