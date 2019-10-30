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

	printf("New termios structure set\n");

	linkLayerData.fd = fd;

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

//WRITER FUNCTIONS

int LLWRITE(int fd , unsigned char *buffer, int length)
{
	unsigned char buf[255];
	unsigned char received[1];
	int sizeBuf = 0;

	stuffing(buffer, length);

	for (int i = 0; i < linkLayerData.sizeFrame; i++)
		printf("%0x ", linkLayerData.frame[i]);

	printf("\n");

	int wrt = write(linkLayerData.fd, linkLayerData.frame, linkLayerData.sizeFrame);
	printf("%d bytes written\n", wrt);
	alarm(3);

	int n = 0;

	do
	{
		//Checking if it has send more than 4 times the packet
		if (linkLayerData.numTransmissions >= 4)
		{
			printf("Didn't get a response. BYE!\n");
			return -1;
		}

		int res = read(linkLayerData.fd, received, 1);

		//If the read is successful cancels the alarm. If not it continues trying to read
		if (res != -1)
			alarm(0);
		else
			continue;

		//Should Have the stateMachine here to confirm when it reachs the end

		stateMachine(&n, received, buf, &sizeBuf);

		if (n == 5)
		{
			printf("\n --- buf2 = %x --\n", buf[2]);
			if (readResponse(linkLayerData.frame[2], buf[2]) != 0) //caso nao tenha recebido bem
			{
				printf(" \n ---- REPEAT ----\n");
				wrt = write(linkLayerData.fd, linkLayerData.frame, linkLayerData.sizeFrame);
				printf("%d bytesrepeated\n", wrt);
				alarm(3);
			}
			else
				break;
			n = -1;
		}

		n++;

	} while (1);

	printf("\n HEREE2 \n");

	return wrt;
}

int stuffing(unsigned char *beforeStuffing, int sizeBeforeStuffing)
{

	//allocating necessary space
	linkLayerData.frame = (unsigned char *)malloc(sizeof(unsigned char) * sizeBeforeStuffing);

	//INITIAl FLAG
	linkLayerData.frame[0] = beforeStuffing[0];

	//current position on the afterStuffing array
	int currentPositionOfStuffing = 1;

	//Size of the afterStuffing array
	linkLayerData.sizeFrame = sizeBeforeStuffing;

	for (int i = 1; i < sizeBeforeStuffing - 1; i++, currentPositionOfStuffing++)
	{

		if (beforeStuffing[i] == FLAG || beforeStuffing[i] == STUFFING)
		{
			linkLayerData.sizeFrame += 2;
			linkLayerData.frame = realloc(linkLayerData.frame, linkLayerData.sizeFrame);

			linkLayerData.frame[currentPositionOfStuffing] = STUFFING;
			linkLayerData.frame[currentPositionOfStuffing + 1] = beforeStuffing[i] ^ EXCLUSIVE_OR_STUFFING;
			currentPositionOfStuffing++;
		}
		else
		{
			linkLayerData.frame[currentPositionOfStuffing] = beforeStuffing[i];
		}
	}

	//END FLAG
	linkLayerData.frame[linkLayerData.sizeFrame - 1] = beforeStuffing[sizeBeforeStuffing - 1];

	return 0;
}

int readResponse(unsigned char originalFlag, unsigned char cFlag)
{
	printf("\n Original Flag: %x \n", originalFlag);
	printf("\n C Flag: %x \n", cFlag);

	//Se for a trama inicial esta a espera de um UA
	if (originalFlag == _SET)
	{
		printf("\n HEREE \n");
		if (cFlag == _UA)
			return 0;
		return 1;
	}
	//send disc, expects disc
	if (originalFlag == _DISC)
	{

		if (cFlag == _DISC)
			return 0;

		return 1;
	}

	if (originalFlag != C_SET && originalFlag != 0x00)
		return 1;

	unsigned char controlBit = originalFlag << 1;

	unsigned char expectedCFlag = _RR | controlBit;

	if (expectedCFlag == cFlag)
		return 0;

	return 1;
}

void atende()
{

	printf("atendeu\n");

	if (linkLayerData.numTransmissions < 3)
	{

		int res = write(linkLayerData.fd, linkLayerData.frame, linkLayerData.sizeFrame);
		printf("%d bytes written\n", res);

		alarm(3);
	}
	linkLayerData.numTransmissions++;
}

// END WRITER FUNCTIONS ===============================

//Receiver functions

int readDataPacketSendResponse(unsigned char **dataPacket, int *sizeDataPacket)
{

	do
	{
		LLREAD(linkLayerData.fd ,dataPacket);

		*sizeDataPacket = linkLayerData.sizeFrame;

		if (checkBCC2(*dataPacket, *sizeDataPacket))
		{
			receivedOK((*dataPacket)[2]);
			printf("Received the info correctly!\n");

			break;
		}

		receivedOK(0x77);
		printf("Something went wrong and the BCC2 is not correct!\n");

	} while (TRUE);
}

int LLREAD(int fd, unsigned char **dataPacket)
{
	int res = 0;
	unsigned char buf[5];

	unsigned char SET[255];
	int n = 0;
	int sizeMessage = 0;

	while (1)
	{
		res = read(fd, buf, 1);

		stateMachine(&n, buf[0], SET, &sizeMessage);

		if (n == 5)
		{
			destuffing(SET, sizeMessage, dataPacket, &linkLayerData.sizeFrame);

			break;
		}
	}

	return 0;
}

void receivedOK(unsigned char controlBit)
{

	unsigned char sendDataPacket[5];

	sendDataPacket[0] = FLAG;
	sendDataPacket[1] = A;

	switch (controlBit)
	{
	case _SET:
		sendDataPacket[2] = _UA;
		break;

	case _DISC:
		sendDataPacket[2] = _DISC;
		break;

	case _UA:
		sendDataPacket[2] = _UA;
		break;

	case 0x40:
		sendDataPacket[2] = _RR;

		sendDataPacket[2] |= controlBit << 1;

		break;

	case 0x00:
		sendDataPacket[2] = _RR;

		sendDataPacket[2] |= controlBit << 1;

		break;

	default:
		sendDataPacket[2] = _REJ;

		sendDataPacket[2] |= controlBit << 1;
		break;
	}

	sendDataPacket[3] = sendDataPacket[1] ^ sendDataPacket[2];
	sendDataPacket[4] = FLAG;

	write(linkLayerData.fd, sendDataPacket, 5);
}

int destuffing(unsigned char *SET, int sizeSET, unsigned char **afterDestuffing, int *sizeAfterDestuffing)
{

	//allocating necessary space
	(*afterDestuffing) = (unsigned char *)malloc(sizeof(unsigned char) * sizeSET);

	//current position on the afterStuffing array
	int currentPositionOfDestuffing = 0;

	//Size of the afterStuffing array
	int newSize = sizeSET;

	for (int i = 0; i < sizeSET; i++, currentPositionOfDestuffing++)
	{

		if (SET[i] == STUFFING)
		{
			newSize -= 2;
			(*afterDestuffing) = realloc((*afterDestuffing), newSize);

			(*afterDestuffing)[currentPositionOfDestuffing] = SET[i + 1] ^ EXCLUSIVE_OR_STUFFING;

			//Advance the second argument of XOR
			i++;
		}
		else
		{
			(*afterDestuffing)[currentPositionOfDestuffing] = SET[i];
		}
	}

	*sizeAfterDestuffing = newSize;

	return 0;
}

void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeMessage)
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
		if (byte_received == C_SET || byte_received == _SET || byte_received == _DISC || byte_received == _UA)
		{
			SET[*sizeMessage] = byte_received;
			*state = 3;
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
		if (byte_received == SET[*sizeMessage - 1] ^ SET[*sizeMessage - 2])
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

		if (byte_received == FLAG)
		{
			*state = 5;
		}
		else
			*state = 4;
		break;
	}

	(*sizeMessage)++;
}

int checkBCC2(unsigned char SET[], int sizeMessage)
{

	unsigned char BCC2 = 0x00;

	for (int i = 4; i < sizeMessage - 2; i++)
	{

		BCC2 ^= SET[i];
	}

	printf("%0x\n", BCC2);
	printf("%0x\n", SET[sizeMessage - 2]);

	if (BCC2 == SET[sizeMessage - 2])
		return TRUE;
	else
		return FALSE;
}

//END RECEIVER FUNCTIONS =============================
