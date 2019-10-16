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

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03
#define BCC A ^ C_SET

volatile int STOP = FALSE;

void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int * sizeMessage)
{
	switch (*state)
	{

	case 0:
		if (byte_received == FLAG)
		{
			SET[*sizeMessage] = byte_received;
			*state = 1;
		}
		break;
	case 1:
		if (byte_received == A)
		{
			SET[*sizeMessage] = byte_received;
			*state = 2;
		}
		else if (byte_received != FLAG)
			*state = 0;
		break;
	case 2:
		if (byte_received == C_SET)
		{
			SET[*sizeMessage] = byte_received;
			*state = 3;
		}
		else if (byte_received == FLAG)
		{
			*state = 1;
		}
		else
			*state = 0;
		break;
	case 3:
		if (byte_received == BCC)
		{
			SET[*sizeMessage] = byte_received;
			*state = 4;
		}
		else if (byte_received == FLAG)
		{
			*state = 1;
		}
		else
			*state = 0;
		break;

	case 4:
		SET[*sizeMessage] = byte_received;
		*state = 5;
		break;
	case 5:
		if (byte_received == FLAG)
		{
			SET[*sizeMessage] = byte_received;
			*state = 6;
		}
		else
			*state = 5;
		break;

	}


	(*sizeMessage)++;
}

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

	unsigned char SET[255];
	int n = 0;
	int sizeMessage = 0;

	while (STOP == FALSE)
	{							/* loop for input */
		res = read(fd, buf, 1); /* returns after 5 chars have been input */
								/* so we can printf... */
		printf("%x\n", buf[0]);

		stateMachine(&n, buf[0], SET, &sizeMessage);

		if (n == 6)
		{
			STOP = TRUE;
		}
	}

	write(fd, SET, sizeMessage);

	sleep(1);

	LLCLOSE(fd, &oldtio);

	return 0;
}
