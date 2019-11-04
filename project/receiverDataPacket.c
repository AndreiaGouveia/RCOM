#include "receiverDataPacket.h"

int getInfoFile(unsigned char *initialDataPacket, int sizeInitialDataPacket, char **nameOfFile, int *sizeOfName, int *sizeOfFile)
{

	for (int i = 5; i < sizeInitialDataPacket; i++)
	{
		//gets size of file
		if (initialDataPacket[i] == 0x00)
		{

			i++;
			int sizeOfValue = initialDataPacket[i];

			int offset = i + 1;

			for (int j = offset; j < (sizeOfValue + offset) && j < sizeInitialDataPacket; j++)
			{

				(*sizeOfFile) = (*sizeOfFile) << 8 * (j - offset);
				(*sizeOfFile) |= initialDataPacket[j];
				i++;
			}
		}
		//gets name of file
		else if (initialDataPacket[i] == 0x01)
		{

			i++;
			*sizeOfName = initialDataPacket[i];

			int offset = i + 1;

			(*nameOfFile) = malloc(sizeof(unsigned char) * (*sizeOfName));

			for (int j = offset; j < ((*sizeOfName) + offset) && j < sizeInitialDataPacket; j++)
			{

				(*nameOfFile)[j - offset] = initialDataPacket[j];
				i++;
			}
		}
	}

	return 0;
}

int getData(unsigned char *dataPacket, unsigned char **fullFile, int beginPosition)
{
	int numOctets = 256*dataPacket[6] + dataPacket[7];

	//parses the data from the dataPacket to the fullFile
	for (int i = 0; i < numOctets; i++)
	{
		(*fullFile)[i + beginPosition] = dataPacket[i+8];
	}

	return numOctets;
}

void createFile( char * nameOfFile, int sizeOfFile, unsigned char * fullFile){
	FILE * finishFile = fopen(nameOfFile, "wb+");//create file

	if(finishFile == NULL)//checks if creation went ok
	{
		perror("Could not create file");
		exit (-1);
	}

	fwrite(fullFile, sizeof(unsigned char), sizeOfFile, finishFile);//parse the info from the fullFile to our final file

}


void progressBar(float percentageReceived){

	printf("\rCompleted: %f[", percentageReceived);

	for(int i = 0; i < percentageReceived; i += 5){
		printf("=");
	}
	printf("]");

	fflush (stdout);
}
