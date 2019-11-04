/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linkLayer.h"
#include "receiverDataPacket.h"

int main(int argc, char **argv)
{
	int fd;

	if ((argc < 2) ||
		((strcmp("/dev/ttyS0", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS1", argv[1]) != 0)))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}

	int porta = atoi(&argv[1][9]);
	fd = LLOPEN(porta, RECEIVER);

	if (fd < 0)
    {
      perror(argv[1]);
      exit(-1);
    }

	printf("Waiting for information!\n");

	//Initial DataPacket (ther real information is coming after this one)
	unsigned char *initialDataPacket;
	int sizeInitialDataPacket = 0;

	recivingInformationDataPacket(fd, &initialDataPacket, &sizeInitialDataPacket, Start);

	//Getting File Info
	char *nameOfFile;
	int sizeOfName = 0;
	int sizeOfFile = 0;

	getInfoFile(initialDataPacket, sizeInitialDataPacket, &nameOfFile, &sizeOfName, &sizeOfFile);

	printf("Starting to received %s with size %d!\n\n", nameOfFile, sizeOfFile);

	int infoReceived = 0;
	int returnValueGetData = 0;
	unsigned char * fullFile = malloc(sizeof(unsigned char)*sizeOfFile);

	//FILE IS COMING
	while (infoReceived < sizeOfFile)
	{
		unsigned char *dataPacket;
		int sizeDataPacket = 0;

		int beginPosition = infoReceived;

		sizeDataPacket = LLREAD(fd, &dataPacket);

		infoReceived += sizeDataPacket - 10;

		int returnValue = getData(dataPacket, &fullFile, beginPosition);
		returnValueGetData += returnValue;

		progressBar(((double )returnValueGetData / sizeOfFile) * 100);
	}

	//Final DataPacket
	unsigned char *endDataPacket;
	int sizeEndDataPacket = 0;
	recivingInformationDataPacket(fd, &endDataPacket, &sizeEndDataPacket, End_);

	//UA DataPacket
	unsigned char *UAdataPacket;
	int sizeUAdataPacket;
	readInfoDataPacket(fd, &UAdataPacket, &sizeUAdataPacket);

	createFile(nameOfFile, sizeOfFile, fullFile);

	printf("\n\nCreated File Sucessfuly! Go check out %s!\n", nameOfFile);

	sleep(1);

	if (LLCLOSE(fd) == -1)
		exit(EXIT_FAILURE);

	return 0;
}
