/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serialPort.h"
#include "writerDataPacket.h"
#include "receiverDataPacket.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x30
#define BCC A ^ C_SET

int main(int argc, char **argv)
{
	int fd, c, res;
	struct termios oldtio, newtio;
	unsigned char buf[5];

	if ((argc < 2) ||
		((strcmp("/dev/ttyS0", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS1", argv[1]) != 0)))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}

	/*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

	fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0)
	{
		perror(argv[1]);
		exit(-1);
	}

	LLOPEN(fd, &newtio, &oldtio);

	unsigned char *initialDataPacket;
	int sizeInitialDataPacket = 0;

	//Initial DataPacket (ther real information is coming after this one)
	LLREAD(fd, &initialDataPacket, &sizeInitialDataPacket);

	char * nameOfFile;
	int sizeOfName = 0;
	int sizeOfFile = 0;

	getSizeFile(initialDataPacket, sizeInitialDataPacket, &nameOfFile, &sizeOfName, &sizeOfFile);

	printf("Name: %s, SizeName: %d, SizeOfFile: %d\n\n", nameOfFile, sizeOfName, sizeOfFile);
	
	int infoRecieved = 0;

	unsigned char *dataPacket;
	int sizeDataPacket = 0;
	int n = 0;

	//FILE IS COMING
	while(infoRecieved <= sizeOfFile)
	{

		unsigned char *dataPacket;
		int sizeDataPacket = 0;


		if(LLREAD(fd, &dataPacket, &sizeDataPacket) == 0)
			infoRecieved += sizeDataPacket - 6;
		
		n++;

		for(int i = 0; i < sizeDataPacket; i++)
			printf("%0x ", dataPacket[i]);

		printf("\nSize of the last data packet: %d\n", sizeDataPacket);
		printf("Received so far: %d, %d\n\n", infoRecieved, n);

	}

	printf("GOT OUT OF CYCLE\n");
	unsigned char *endDataPacket;
	int sizeEndDataPacket = 0;

	LLREAD(fd, &endDataPacket, &sizeEndDataPacket);
	
	getSizeFile(endDataPacket, sizeEndDataPacket, &nameOfFile, &sizeOfName, &sizeOfFile);
	printf("Name: %s, SizeName: %d, SizeOfFile: %d\n", nameOfFile, sizeOfName, sizeOfFile);
	
	sleep(1);

	LLCLOSE(fd, &oldtio);

	return 0;
}
