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

int LLOPEN( int porta, enum Status status))
{
	int fd;

	char * portaSerie = getPort(porta);

	if (status == TRANSMITTER)
	{
		fd = open(portaSerie, O_RDWR | O_NOCTTY | O_NONBLOCK);
	}
	else if (status == RECEIVER)
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

void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeSET)
{
    switch (*state)
    {

    case 0:
        if (byte_received == FLAG)
        {
            SET[*sizeSET] = byte_received;
            *state = 1;

    (*sizeSET)++;
        }
        break;
    case 1:
        if (byte_received == A)
        {
            SET[*sizeSET] = byte_received;
            *state = 2;

    (*sizeSET)++;
        }
        else if (byte_received != FLAG)
            *state = 0;
        break;
    case 2:
        if (byte_received == C_SET || byte_received == _SET || byte_received == _DISC || byte_received == _UA 
	|| (byte_received == _RR || C_SET << 1) || (byte_received == _REJ || C_SET << 1) || byte_received == _RR || byte_received == _REJ)
        {
            SET[*sizeSET] = byte_received;
            *state = 3;

    (*sizeSET)++;
        }
        else if (byte_received == FLAG)
        {
            *state = 1;
        }
        else
        {
            *state = 0;
        }
        break;
    case 3:
        if ((byte_received == SET[*sizeSET - 1]) ^ SET[*sizeSET - 2])
        {
            SET[*sizeSET] = byte_received;
            *state = 4;

    (*sizeSET)++;
        }
        else if (byte_received == FLAG)
        {
            *state = 1;
        }
        else
            *state = 0;
        break;
    case 4:
        SET[*sizeSET] = byte_received;
    (*sizeSET)++;

        if (byte_received == FLAG)
        {
            *state = 5;
        }
        else
            *state = 4;
        break;
    }


}

