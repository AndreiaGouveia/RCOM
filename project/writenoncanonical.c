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

	if ((argc < 2) ||
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

	LLOPEN(fd, &newtio, &oldtio);

	char data[] = "aqui";

    getSETDataPacket(data, 5);

	//Sending First DataPacket
	LLWRITE(fd, data, 5);

	LLCLOSE(fd, &oldtio);

	return 0;
}
