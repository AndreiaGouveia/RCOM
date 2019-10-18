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
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}

	int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		perror(argv[1]);
		exit(-1);
	}

	// Getting our file
	FILE *file = fopen(argv[2],"rb");

	if(file==NULL)
	{
		perror("Can't find such file");
		exit(-1);
	}	

	//storing info
	size_t fileSize;
	unsigned char * fullData = readFile(file , &fileSize,argv[2]);
	//test 
	printf("\n data : %s, with size of %ld\n",fullData, fileSize);

	LLOPEN(fd, &newtio, &oldtio);

	//=====Send Start====  DA ERRO DE SEGMENTATION FAULT :( )
	/*unsigned char * start;
	start = getInitialEndDataPacket(file, argv[2], Begin,(long int)fileSize);*/
	//printf("\nsize:============%c=========\n",start[2]);
	//LLWRITEstartEnd(fd, start, (int)strlen(start));
	//LLWRITEstartEnd(fd, start, sizeof(start));


	//=====Send FILE Data=====
	unsigned char tempData[100];
	unsigned char *dataPacket;
	int indice=0;

	for(size_t i=0 ; i < fileSize; i++)
	{
		tempData[indice] = fullData[i];
		indice=(i+1)%98;
		if(indice == 97 || ((i+1)==fileSize))//<=100 bytes at a time
    	{
		
			tempData[indice] = fullData[i+1];
			tempData[indice+1] = '\0';
			int size = (int)strlen(tempData);
			printf("\n data size: %d",size);
			dataPacket = getSETDataPacket(tempData, size);
			LLWRITE(fd, dataPacket, strlen(dataPacket));
			printf("\nIndice: %ld line:||%s||\n", i,tempData);//ver o que manda em cada packet
		}
	}

	LLCLOSE(fd, &oldtio);

	return 0;
}
