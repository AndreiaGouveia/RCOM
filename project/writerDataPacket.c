#include "writerDataPacket.h"

#include <math.h>

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
    int sizeOfNumberFileSize = ceil(log2(fileSize)/8.0);
    int sizeOfName = strlen(fileName) + 1;
    (*sizeInitialSet) = (sizeOfNumberFileSize + 5 + sizeOfName) * sizeof(unsigned char);
    (*initialSet) = (unsigned char *)malloc((*sizeInitialSet));

    if (cf == Begin)
    {
        (*initialSet)[0] = _SET;
    }
    else
        (*initialSet)[0] = _DISC;

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


