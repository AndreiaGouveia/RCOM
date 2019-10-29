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

#define RECEIVER 0
#define TRANSMITTER 1

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x40
#define BCC A ^ C_SET

int main(int argc, char **argv)
{
	int fd, c, res;
	unsigned char buf[5];

	if ((argc < 2) ||
		((strcmp("/dev/ttyS0", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS1", argv[1]) != 0)))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}

	fd = LLOPEN(argv[1], RECEIVER);

	if (fd < 0)
    {
      perror(argv[1]);
      exit(-1);
    }

	unsigned char *initialDataPacket;
	int sizeInitialDataPacket = 0;


	//Initial DataPacket (ther real information is coming after this one)
	while (LLREAD(&initialDataPacket, &sizeInitialDataPacket) == 1);

	char *nameOfFile;
	int sizeOfName = 0;
	int sizeOfFile = 0;


	//Checks if the control field is correct. In this case 0x02 -> beginning of transaction
	if(initialDataPacket[4] != 0x02) {
		printf("The control field of the initial control packet should be 0x02.");
		return 1;
	}

	getSizeFile(initialDataPacket, sizeInitialDataPacket, &nameOfFile, &sizeOfName, &sizeOfFile);

	printf("Name: %s, SizeName: %d, SizeOfFile: %d\n\n", nameOfFile, sizeOfName, sizeOfFile);

	int infoReceived = 0;

	int expectedNumSeq = 0;
	int returnValueGetData = 0;

	unsigned char * fullFile = malloc(sizeof(unsigned char)*sizeOfFile);

	//FILE IS COMING
	while (infoReceived < sizeOfFile)
	{
		unsigned char *dataPacket;
		int sizeDataPacket = 0;

		int beginPosition = infoReceived;

		if (LLREAD(&dataPacket, &sizeDataPacket) == 0)
			infoReceived += sizeDataPacket - 10;

		expectedNumSeq ++;


		//Checks if the control field is correct. In this case 0x01 -> data packet transaction.
		if(dataPacket[4] != 0x01) {
			printf("Control field of data packets should be 0x01.");
			return 1;
		}


		//Checks if the sequence number of the packet matches the nº of the packet received.
		if(dataPacket[5] != expectedNumSeq ) {
			printf("Sequence number doesn't match.");
			return 1;
		}
		

		int returnValue = getData(dataPacket, sizeDataPacket, &fullFile, beginPosition);
		returnValueGetData += returnValue;

		for (int i = 0; i < sizeDataPacket; i++)
			printf("%0x ", dataPacket[i]);

		printf("\nSize of the last data packet: %d\n", sizeDataPacket);
		printf("Received so far: %d, %d, %d\n\n", infoReceived, expectedNumSeq, returnValueGetData);
	}

	printf("GOT OUT OF CYCLE\n");


	for (int i = 0; i < sizeOfFile; i++)
		printf("%0x ", fullFile[i]);

	unsigned char *endDataPacket;
	int sizeEndDataPacket = 0;

	while(LLREAD(&endDataPacket, &sizeEndDataPacket)==1);
	
	char * nameOfFileEND;
	int sizeOfNameEND = 0;
	int sizeOfFileEND = 0;

	//Checks if the control field is correct. In this case 0x03 -> end of transaction
	if(endDataPacket[4] != 0x03) {
		printf("The control field of the end control packet should be 0x03.");
		return 1;
	}

	unsigned char *UAdataPacket;
	int sizeUAdataPacket = 0;

	printf("aqui!\n");

	//while(LLREAD(&UAdataPacket, &sizeUAdataPacket)==1);

	getSizeFile(initialDataPacket, sizeInitialDataPacket, &nameOfFileEND, &sizeOfNameEND, &sizeOfFileEND);

	printf("Name: %s, SizeName: %d, SizeOfFile: %d\n\n", nameOfFileEND, sizeOfNameEND, sizeOfFileEND);

	
	FILE * finishFile = fopen(nameOfFile, "wb+");

	if(finishFile == NULL)
	{
		perror("Could not create file");
		exit (-1);
	}

	fwrite(fullFile, sizeof(unsigned char), sizeOfFile, finishFile);
	
	sleep(1);

	if (LLCLOSE(fd) == -1)
		exit(EXIT_FAILURE);

	return 0;
}
