/*Non-Canonical Input Processing*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "serialPort.h"
#include "sendDataPacket.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define SIZE_DATA 100

volatile int STOP = FALSE;

unsigned char buf[255];

int main(int argc, char **argv)
{

	struct termios oldtio, newtio;
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

	int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		perror(argv[1]);
		exit(-1);
	}

	// Getting our file
	FILE *file = fopen(argv[2], "rb");

	if (file == NULL)
	{
		perror("Can't find such file");
		exit(-1);
	}

	//storing info
	size_t fileSize;
	unsigned char *fullData = readFile(file, &fileSize, argv[2]);

	LLOPEN(fd, &newtio, &oldtio);

	//=====Send Start====  DA ERRO DE SEGMENTATION FAULT :( )
	unsigned char * start;
	start = getInitialEndDataPacket(file, argv[2], Begin, (long int)fileSize);
	LLWRITE(fd, start, 6 * sizeof(unsigned char) + strlen(argv[2]));

	//=====Send FILE Data=====
	unsigned char tempData[100];
	unsigned char *dataPacket;
	int indice = 0;

	for (size_t i = 0; i < fileSize; i += 100)
	{
		dataPacket = getSETDataPacket(*fullData[i], 100);

		LLWRITE(fd, dataPacket, 100);

		for (int j = i; j < 100; j++)
			printf("%0x\n", dataPacket[j]);

		if (i + 100 >= fileSize)
		{
			dataPacket = getSETDataPacket(*fullData[i], fileSize - i);

			LLWRITE(fd, dataPacket, fileSize - i);

			for (int j = i; j < fileSize - i; j++)
				printf("%0x\n", dataPacket[j]);
		}
	}

	LLCLOSE(fd, &oldtio);

	return 0;
}
