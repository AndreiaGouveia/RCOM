#include "writerDataPacket.h"

#include <math.h>

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

unsigned char * getSETDataPacket(unsigned char *data, int sizeData)
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

    getBCC2(data, sizeData, &setBefore[sizeData + 4]);

    setBefore[sizeData + 5] = FLAG;

    return setBefore;
}

void LLWRITE(int fileDiscriptor, unsigned char *package, int packageSize)
{
    fd = fileDiscriptor;
    unsigned char buf[255];

    stuffing(package, packageSize);

    int res = write(fd, set, sizeSet);
    printf("%d bytes written\n", res);
    alarm(3);

    //Quando receber a trama de resposta toda
    unsigned char temp =_RR;
    temp |= set[2] <<1;

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

        //Should Have the stateMachine here to confirm when it reachs the end
       
        if(n != 0 && buf[n] == FLAG)
        {
            if(buf[2] != temp) //caso nao tenha recebido bem
            {
                printf(" \n ---- REPEAT ----\n");
                int res = write(fd, set, sizeSet);
                printf("%d bytesrepeated\n", res);
                alarm(3);
            }
            else  {
                break;
            }

            n=-1;
        }
        
        n++;

    } while (1);

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

int getInitialEndDataPacket(FILE *fileToBeSent, char fileName[], enum WhichControl cf, int fileSize, unsigned char ** initialSet, int * sizeInitialSet)
{
    

    //alocating the space for the initialSet
    int sizeOfNumberFileSize = 2; // ceil(ln(fileSize, 2)/8.0);
    int sizeOfName = strlen(fileName) + 1;
    (*sizeInitialSet) = (sizeOfNumberFileSize + 5 + sizeOfName) * sizeof(unsigned char);
    (*initialSet) = (unsigned char *)malloc((*sizeInitialSet));

    if (cf == Begin)
    {
        (*initialSet)[0] = 0x02;
    }
    else
        (*initialSet)[0] = 0x03;

    //TYPE - LENGTH - VALUE
    
    //Size of file
    (*initialSet)[1] = 0x00;


    (*initialSet)[2] = sizeOfNumberFileSize;

    int i = (*initialSet)[2] + 2;
    int n = 0;

    while (i > 2) { 
  
        (*initialSet)[i] = (fileSize >> 8*n) & 0xFF;
        
        i--;
        n++;
    } 

    int offset = (*initialSet)[2]+2;

    //Name of file
    (*initialSet)[1+offset] = 0x01;
    (*initialSet)[2+offset] = sizeOfName;

    for (int i = 0; i < sizeOfName; i++)
    {
        (*initialSet)[i + 3 + offset] = fileName[i];
    }

    return 0;
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

int stuffing(unsigned char * beforeStuffing, int sizeBeforeStuffing){

    //allocating necessary space
    set = (unsigned char *) malloc(sizeof(unsigned char) * sizeBeforeStuffing);

    //INITIAl FLAG
    set[0] = beforeStuffing[0];

    //current position on the afterStuffing array
    int currentPositionOfStuffing = 1;

    //Size of the afterStuffing array
    sizeSet=sizeBeforeStuffing;

    for(int i = 1; i < sizeBeforeStuffing-1; i++, currentPositionOfStuffing++){

        if(beforeStuffing[i] == FLAG || beforeStuffing[i] == STUFFING)
        {
            sizeSet += 2;
            set=realloc(set, sizeSet);

            set[currentPositionOfStuffing]=STUFFING;
            set[currentPositionOfStuffing + 1] = beforeStuffing[i] ^ EXCLUSIVE_OR_STUFFING;
            currentPositionOfStuffing++;

        }else
        {
            set[currentPositionOfStuffing]=beforeStuffing[i];
        }
        
    }

    //END FLAG
    set[sizeSet-1] = beforeStuffing[sizeBeforeStuffing- 1];

    return 0;
}

