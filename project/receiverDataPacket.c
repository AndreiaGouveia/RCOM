#include "receiverDataPacket.h"

int destuffing(unsigned char * SET, int sizeSET, unsigned char * * afterDestuffing, int * sizeAfterDestuffing){

    //allocating necessary space
    (* afterDestuffing) = (unsigned char *) malloc(sizeof(unsigned char) * sizeSET);

    //INITIAl FLAGs
    (* afterDestuffing)[0] = SET[0];

    //current position on the afterStuffing array
    int currentPositionOfDestuffing = 1;

    //Size of the afterStuffing array
    int newSize=sizeSET;

    for(int i = 1; i < sizeSET-1; i++, currentPositionOfDestuffing++){

        if(SET[i] == STUFFING)
        {
            newSize -= 2;
            (* afterDestuffing)=realloc((* afterDestuffing), newSize);

            (* afterDestuffing)[currentPositionOfDestuffing] = SET[i+1] ^ EXCLUSIVE_OR_STUFFING;

            //Advance the second argument of XOR
            i++;

        }else
        {
            (* afterDestuffing)[currentPositionOfDestuffing]=SET[i];
        }
        
    }

    
    //END FLAG
    (* afterDestuffing)[newSize-1] = SET[sizeSET - 1];

    *sizeAfterDestuffing=newSize;

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
		{
			*state = 0;
		}
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
		SET[*sizeMessage] = byte_received;

		if (byte_received == FLAG)
		{
			*state = 6;
		}
		else
			*state = 5;
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

	if (BCC2 == SET[sizeMessage - 2])
		return TRUE;
	else
		return FALSE;
}

void receivedOK(int fd, enum ControlField cf, unsigned char controlBit)
{

    unsigned char sendDataPacket[5];

    sendDataPacket[0] = FLAG;
    sendDataPacket[1] = A;

    switch (cf)
    {
    case SET:
        sendDataPacket[2] = _SET;
        break;

    case DISC:
        sendDataPacket[2] = _DISC;
        break;

    case UA:
        sendDataPacket[2] = _UA;
        break;

    case RR:
        sendDataPacket[2] = _RR;

        sendDataPacket[2] |= controlBit << 1;
        break;

    case REJ:
        sendDataPacket[2] = _REJ;

        sendDataPacket[2] |= controlBit << 1;
        break;
    }

    sendDataPacket[3] = sendDataPacket[1] ^ sendDataPacket[2];
    sendDataPacket[4] = FLAG;

    write(fd, sendDataPacket, 5);
}