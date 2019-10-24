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

int getFullDataPacket(unsigned char *data, int sizeData, unsigned char ** fullData, int * sizefullData, int nSeq){

	(* sizefullData) = sizeData+4;

	(*fullData) = malloc(sizeof(unsigned char) * (*sizefullData));
	
	(*fullData)[0] = 0x01;
	(*fullData)[1] = nSeq;
	(*fullData)[2] = sizeData/256;
	(*fullData)[3] = sizeData % 256;

	//memcpy(dest, src, strlen(src)+1);

	for(int i = 0; i < sizeData; i++)
		(*fullData)[i+4] = data[i];
}

unsigned char *getSETDataPacket(unsigned char *data, int sizeData , unsigned char CFlag)
{
    unsigned char *setBefore = (unsigned char *)malloc((sizeData + 6) * sizeof(unsigned char));

    setBefore[0] = FLAG;
    setBefore[1] = A;
    setBefore[2] = CFlag;
    setBefore[3] = setBefore[1] ^ setBefore[2]; //BCC1

    for (int i = 1; i <= sizeData; i++)
    {
        setBefore[i + 3] = data[i - 1];
    }

    getBCC2(data, sizeData, &setBefore[sizeData + 4]);

    setBefore[sizeData + 5] = FLAG;

    return setBefore;
}

int readResponse(unsigned char originalFlag, unsigned char cFlag)
{
	printf("\n Original Flag: %x \n", originalFlag);
	printf("\n C Flag: %x \n", cFlag);

	//Se for a trama inicial esta a espera de um UA
	if(originalFlag == _SET)
	{
		printf("\n HEREE \n");
		if(cFlag == _UA)
			return 0;
		return 1;
		
	}
	//send disc, expects disc
	if(originalFlag == _DISC){

		if(cFlag == _DISC)
			return 0;

		return 1;
	}


	if(originalFlag != C_SET && originalFlag != 0x00)
		return 1;


	unsigned char controlBit = originalFlag << 1;

	unsigned char expectedCFlag = _RR | controlBit;

	if(expectedCFlag == cFlag)
		return 0;

	return 1;
 


	
}

//ALTERAR VALOR DE RETORNO PARA NUMERO DE CARARTERES ESCRITOS
int LLWRITE(int fd1, unsigned char *buffer, int length)
{
fd = fd1;

    unsigned char buf[255];

    stuffing(buffer, length);

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
            return -1;
        }

        res = read(fd, &buf[n], 1);

        //If the read is successful cancels the alarm. If not it continues trying to read
        if (res != -1)
            alarm(0);
        else
            continue;

        //Should Have the stateMachine here to confirm when it reachs the end

        if (n != 0 && buf[n] == FLAG)
        {
		printf("\n --- buf2 = %x --\n", buf[2]);
            if (readResponse(set[2], buf[2]) != 0) //caso nao tenha recebido bem
            {
                printf(" \n ---- REPEAT ----\n");
                int res = write(fd, set, sizeSet);
                printf("%d bytesrepeated\n", res);
                alarm(3);
            }
            else
                break;
            n = -1;
        }

        n++;

    } while (1);

	printf("\n HEREE2 \n");

    return 0;
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

int getInitialEndDataPacket(FILE *fileToBeSent, char fileName[], enum WhichControl cf, int fileSize, unsigned char **initialSet, int *sizeInitialSet)
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

    while (i > 2)
    {

        (*initialSet)[i] = (fileSize >> 8 * n) & 0xFF;

        i--;
        n++;
    }

    int offset = (*initialSet)[2] + 2;

    //Name of file
    (*initialSet)[1 + offset] = 0x01;
    (*initialSet)[2 + offset] = sizeOfName;

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

int stuffing(unsigned char *beforeStuffing, int sizeBeforeStuffing)
{

    //allocating necessary space
    set = (unsigned char *)malloc(sizeof(unsigned char) * sizeBeforeStuffing);

    //INITIAl FLAG
    set[0] = beforeStuffing[0];

    //current position on the afterStuffing array
    int currentPositionOfStuffing = 1;

    //Size of the afterStuffing array
    sizeSet = sizeBeforeStuffing;

    for (int i = 1; i < sizeBeforeStuffing - 1; i++, currentPositionOfStuffing++)
    {

        if (beforeStuffing[i] == FLAG || beforeStuffing[i] == STUFFING)
        {
            sizeSet += 2;
            set = realloc(set, sizeSet);

            set[currentPositionOfStuffing] = STUFFING;
            set[currentPositionOfStuffing + 1] = beforeStuffing[i] ^ EXCLUSIVE_OR_STUFFING;
            currentPositionOfStuffing++;
        }
        else
        {
            set[currentPositionOfStuffing] = beforeStuffing[i];
        }
    }

    //END FLAG
    set[sizeSet - 1] = beforeStuffing[sizeBeforeStuffing - 1];

    return 0;
}
