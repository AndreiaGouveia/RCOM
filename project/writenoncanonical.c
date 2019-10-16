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

unsigned char setInit[4];
unsigned char setEnd[2];

int FLAG_ALARM = 1;
int count_ALARM = 0;
int fd, c, res;

void atende()
{

	printf("atendeu\n");

	if (FLAG_ALARM == 1 && count_ALARM <= 3)
	{

		if (count_ALARM < 3)
		{

			sendInitialDataPacket(fd, setInit);

			sendEndDataPacket(fd, setEnd);
			
			alarm(3);
		}
		count_ALARM++;
	}
}

int main(int argc, char **argv)
{

	//CRIACAO DO SET

	getSETOfInitialDataPacket(setInit);
	getSETOfFinalDataPacket(setEnd, NULL, 0);

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

	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		perror(argv[1]);
		exit(-1);
	}

	OpenSerialPort(fd, &newtio, &oldtio);

	/*
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
    o indicado no gui�o
  */

	sendInitialDataPacket(fd, setInit);

	sendEndDataPacket(fd, setEnd);

	alarm(3);
	//write

	int n = 0;

	//read
	do
	{

		if (count_ALARM >= 4)
		{
			printf("Didn't get a response. BYE!\n");
			break;
		}

		res = read(fd, &buf[n], 1);

		if (res != -1)
		{
			FLAG_ALARM = 0;
			//signal(SIGALRM, SIG_IGN);
		}
		else
			continue;

		printf("%x\n", buf[n]);

		if (n != 0)
		{
			if (buf[n] == FLAG)
				break;
		}
		n++;

	} while (1);

	CloseSerialPort(fd, &oldtio);

	return 0;
}
