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

void getBCC2(unsigned char *data, int sizeData, unsigned char * BBC2)
{

    *BBC2 = 0x00;
    for (int i = 0; i < sizeData; i++)
    {
        *BBC2 = *BBC2 ^ data[i];
    }
}

unsigned char * getSETDataPacket(unsigned char *data, int sizeData)
{
    set = (unsigned char *)malloc((sizeData + 6) * sizeof(unsigned char));

    set[0] = FLAG;
    set[1] = A;
    set[2] = C_SET;
    set[3] = set[1] ^ set[2]; //BCC1

    for(int i = 1; i <= sizeData; i++)
        {
            set[i + 3] = data[i - 1];
        }

    getBCC2(data, sizeData, &set[sizeData + 4]);
    set[sizeData + 5] = FLAG;

    printf("print do set : %s",set);

    return set;
}

void LLWRITE(int fileDiscriptor, unsigned char *package, int packageSize)
{
    fd=fileDiscriptor;
    unsigned char buf[255];

    int res = write(fd, package, packageSize);
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

void receivedOK(int fd, enum ControlField cf, unsigned char controlBit){

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

    sendDataPacket[3] = sendDataPacket[1]^sendDataPacket[2];
    sendDataPacket[4] = FLAG;


	write(fd, sendDataPacket, 5);
}


long int findSize(FILE * fp) 
{  
     
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int res = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return res; 
} 

unsigned char * getInitialEndDataPacket(FILE * fileToBeSent, char fileName[], enum WhichControl cf,int fileSize){

    //alocating the space for the initialSet
    int initialSize =6*sizeof(unsigned char)+strlen(fileName);
    printf("\n  Size of packetss : %d \n",initialSize);
    unsigned char * initialSet = (unsigned char *) malloc(initialSize);

printf("\n size of initial set: %ld\n",strlen(initialSet));
  if(cf == Begin)
       { 
           initialSet[0] = 0x02;
           printf("\n on begin");
       }
    else
        initialSet[0] = 0x03;

    //Size of file
    initialSet[1] = 0x00;
    initialSet[2] = 0x01;
    initialSet[3] = fileSize;

    //Name of file
    initialSet[4] = 0x01;
    initialSet[5] = strlen(fileName);
    printf("\n size filename: %c",initialSet[9]);

    for(int i = 0; i < strlen(fileName); i++)
        {
            initialSet[i + 6] = fileName[i];
            initialSet[strlen(fileName) + 10] ^= fileName[i]; //get bcc2
        }

    printf("\n PACKET: %s\n",initialSet);
    return initialSet;
}

unsigned char * readFile(FILE * file, size_t * size, unsigned char *fileName)
{
    struct stat fileInfo;

    stat((char*)fileName,&fileInfo);//gets file info

    (*size) = fileInfo.st_size;//gets size of file

    unsigned char * fullData = (unsigned char *)malloc(*size);

    fread(fullData,sizeof(unsigned char),*size,file);//reads file and store it in fullData

    return fullData;
}