#include "linkLayer.h"

struct termios oldtio, newtio;

char * getPort(int porta){
	switch (porta)
	{
	case 0:
		return COM0;
		break;

	case 1:
		return COM1;
		break;

	case 2:
		return COM2;
		break;

	case 3:
		return COM3;
		break;

	case 4:
		return COM4;
		break;

	case 5:
		return COM5;
		break;

	case 6:
		return COM6;
		break;

	case 7:
		return COM7;
		break;							
	
	default :
		printf("porta %d", porta);
		perror ("Port does not exist");
		exit(-1);
		break;
	}
}

int LLOPEN( int porta, int flag)
{
	int fd;

	char * portaSerie = getPort(porta);

	if (flag == TRANSMITTER)
	{
		fd = open(portaSerie, O_RDWR | O_NOCTTY | O_NONBLOCK);
	}
	else if (flag == RECEIVER)
	{
		fd = open(portaSerie, O_RDWR | O_NOCTTY);
	}

	/*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

	if (tcgetattr(fd, &oldtio) == -1)
	{ /* save current port settings */
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(struct termios));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1;  /* blocking read until 5 chars received */
							/*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) prximo(s) caracter(es)
  */

	tcflush(fd, TCIOFLUSH); //fazer fluish antes de mandar cenas

	if (tcsetattr(fd, TCSANOW, &newtio) == -1)
	{
		perror("tcsetattr");
		exit(-1);
	}

	printf("New termios structure set!\n");

	return fd;
}

int LLCLOSE(int fd)
{

	if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
	{
		perror("tcsetattr");
		return -1;
	}

	close(fd);
	return 0;
}

