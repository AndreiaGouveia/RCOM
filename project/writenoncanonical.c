/*Non-Canonical Input Processing*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "utilities.h"
#include "linkLayer.h"
#include "writerDataPacket.h"

int main(int argc, char **argv)
{

	(void)signal(SIGALRM, atende);

	if ((argc < 3) ||
		((strcmp("/dev/ttyS0", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS1", argv[1]) != 0) && (strcmp("/dev/ttyS2", argv[1]) != 0) &&
		 (strcmp("/dev/ttyS4", argv[1]) != 0) &&(strcmp("/dev/ttyS5", argv[1]) != 0)  &&
		 (strcmp("/dev/ttyS6", argv[1]) != 0) &&(strcmp("/dev/ttyS7", argv[1]) != 0) ))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 name_of_file\n");
		exit(1);
	}
	
	int porta = atoi(&argv[1][9]);
	int fd = LLOPEN(porta, TRANSMITTER);

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
	
	unsigned char *fullData = readFile(file, &fileSize, (unsigned char *) argv[2]);

	printf("Sending Information!\n");
	printf("Starting to received %s with size %f!\n\n", argv[2], (double) fileSize);

	sendControlDataPacket(fd, Begin, argv[2], fileSize);

	clock_t fileReceiveStart = clock();

	sendFileData(fd, fileSize, fullData);
	
	clock_t fileReceiveEnd = clock();
	data_link_statistics.timeSpent = ((double) (fileReceiveEnd - fileReceiveStart)) / CLOCKS_PER_SEC;

	sendControlDataPacket(fd, End, argv[2], fileSize);

	sendUA(fd);


	printf("\n\nSent File Sucessfuly! Go check out %s!\n", argv[2]);

	if (LLCLOSE(fd)==-1)
		exit(EXIT_FAILURE);

	printStatistics();

	return 0;
	
}
