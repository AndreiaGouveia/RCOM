/*Non-Canonical Input Processing*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "linkLayer.h"
#include "writerDataPacket.h"
unsigned char buf[255];

int main(int argc, char **argv)
{
	int i, sum = 0, speed = 0;

	(void)signal(SIGALRM, atende);

	if ((argc < 3) ||
		((strcmp("/dev/ttyS0", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS1", argv[1]) != 0) && (strcmp("/dev/ttyS2", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS4", argv[1]) != 0)))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 name_of_file\n");
		exit(1);
	}

	int fd = LLOPEN(argv[1], TRANSMITTER);

	if (fd < 0)
    {
      perror(argv[1]);
      exit(-1);
    }

	//storing info
	size_t fileSize;
	FILE * file = fopen(argv[2], "rb");

	if(file == NULL){
		perror("Can't find such file");
		exit(-1);

	}
	
	unsigned char *fullData = readFile(file, &fileSize, argv[2]);

	//=====Send Start====
	unsigned char *startData;
	int sizeStartData = 0;

	getControlDataPacket(file, argv[2], Begin, fileSize, &startData, &sizeStartData);

	printf("SizeFile: %ld\n", fileSize);

	unsigned char *setStart = getSETDataPacket(startData, sizeStartData, _SET);

	LLWRITE(setStart, 6 + sizeStartData);

	//=====Send FILE Data=====
	unsigned char tempData[100];
	unsigned char *dataPacket;
	int indice = 0;

	int counter = 0;

	for (int i = 0; i < fileSize; i += SIZE_DATA)
	{
		indice++;
		unsigned char * fulldataPacket;
		int sizefullDataPacket;

		getFullDataPacket(&fullData[i], SIZE_DATA, &fulldataPacket, &sizefullDataPacket, indice);
		dataPacket = getSETDataPacket(fulldataPacket, sizefullDataPacket, C_SET);

		LLWRITE(dataPacket, sizefullDataPacket + 6);

		//prints para ver a quantidade de info que manda!
		counter += SIZE_DATA;

		for (int j = 0; j < SIZE_DATA + 6; j++) //ciclo para visualizaç~ao do packet
			printf("%0x ", dataPacket[j]);

		printf("\nWrote so far: %d, %d\n\n", counter, i);

		if ((counter + SIZE_DATA) >= fileSize)
			break;
	}

	printf("SAIU DO CICLO!\n\n");
	//In case that the size file is not a multiple of size_data we need to send the remaining bytes
	if ((fileSize % SIZE_DATA) != 0)
	{

		indice++;
		unsigned char * fulldataPacket;
		int sizefullDataPacket;

		getFullDataPacket(&fullData[fileSize - (fileSize % SIZE_DATA)], fileSize % SIZE_DATA, &fulldataPacket, &sizefullDataPacket, indice);
		dataPacket = getSETDataPacket(fulldataPacket, sizefullDataPacket, C_SET);

		LLWRITE(dataPacket, sizefullDataPacket + 6);

		counter += fileSize % SIZE_DATA;

		for (int j = 0; j < fileSize % SIZE_DATA + 6; j++)
			printf("%0x ", dataPacket[j]);

		printf("\nWrote so far: %d\n\n", counter);
	}

	//=====Send FINAL====
	unsigned char *endData;

	int sizeEndData = 6 + strlen(argv[2]);

	getControlDataPacket(file, argv[2], End, fileSize, &endData, &sizeEndData);

	unsigned char *setEnd = getSETDataPacket(endData, sizeEndData, _DISC);

	LLWRITE(setEnd, 6 + sizeEndData);


	//======= UA Control DATA PACKET
	unsigned char * UAControl;
	int sizeUAControl = 6;
	UAControl = getSETDataPacket(NULL, 0, _UA);

	write(linkLayerData.fd, UAControl, sizeUAControl);

	if (LLCLOSE(fd)==-1)
		exit(EXIT_FAILURE);

	return 0;
}
