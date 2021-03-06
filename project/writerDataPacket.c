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

	(* sizefullData) = sizeData+5;

	(*fullData) = malloc(sizeof(unsigned char) * (*sizefullData));
	
	(*fullData)[0] = 0x01;
	(*fullData)[1] = nSeq;
	(*fullData)[2] = sizeData/256;
	(*fullData)[3] = sizeData % 256;

	for(int i = 0; i < sizeData; i++)
		(*fullData)[i+4] = data[i];

    
    getBCC2((*fullData), (* sizefullData) - 1, &(*fullData)[sizeData + 4]);

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

int getControlDataPacket(char fileName[], enum WhichControl cf, int fileSize, unsigned char **initialSet, int *sizeInitialSet)
{

    //alocating the space for the initialSet
    int sizeOfNumberFileSize = ceil(log2(fileSize)/8.0);
    int sizeOfName = (int) strlen((char *) fileName) + 1;
    (*sizeInitialSet) = (sizeOfNumberFileSize + 6 + sizeOfName) * sizeof(unsigned char);
    (*initialSet) = (unsigned char *)malloc((*sizeInitialSet));

    //check if the control packet is the first or the last
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

    getBCC2((*initialSet), (*sizeInitialSet) - 1, (*initialSet) + offset + sizeOfName + 3);

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

void sendUA(int fd){

	unsigned char * UAControl;
	int sizeUAControl = 5;
	UAControl = getSETDataPacket(NULL, 0, _UA);

	write(fd, UAControl, sizeUAControl);
}

void sendControlDataPacket(int fd, enum WhichControl cf, char * fileName, int fileSize){
	unsigned char *endData;

	int sizeEndData = 6 + (int) strlen((char *) fileName);
	getControlDataPacket(fileName, cf, fileSize, &endData, &sizeEndData);

	if(LLWRITE(fd,endData, sizeEndData)<0)
	{
		perror("\nLLWRITE went wrong");
		exit(-1);
	}

}

void sendFileData(int fd, int fileSize, unsigned char * fullData){
    	//=====Send FILE Data=====
	int indice = 0;
	int counter = 0;

	for (int i = 0; i < fileSize; i += SIZE_DATA)
	{
		
		unsigned char * fulldataPacket;
		int sizefullDataPacket;

		getFullDataPacket(&fullData[i], SIZE_DATA, &fulldataPacket, &sizefullDataPacket, indice);

		if(LLWRITE(fd,fulldataPacket, sizefullDataPacket)<0)
		{
		perror("\nLLWRITE went wrong");
		exit(-1);
		}

		//prints para ver a quantidade de info que manda!
		counter += SIZE_DATA;

		if ((counter + SIZE_DATA) >= fileSize)
			break;
		
		indice++;

        progressBar((double) (counter * 100)/ fileSize);
	}

	//In case that the size file is not a multiple of size_data we need to the remaining bytes
	if ((fileSize % SIZE_DATA) != 0)
	{

        indice++;

		unsigned char * fulldataPacket;
		int sizefullDataPacket;

		getFullDataPacket(&fullData[fileSize - (fileSize % SIZE_DATA)], fileSize % SIZE_DATA, &fulldataPacket, &sizefullDataPacket, indice);
		
		if(LLWRITE(fd,fulldataPacket, sizefullDataPacket)<0)
		{
		perror("\nLLWRITE went wrong");
		exit(-1);
		}

		counter += fileSize % SIZE_DATA;

        progressBar((double) (counter * 100)/ fileSize);
	}

}

