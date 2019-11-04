#include "linkLayerWriter.h"
#include "linkLayer.h"

//WRITER FUNCTIONS

int LLWRITE(int fd , unsigned char *buffer, int length)
{

	linkLayerData.fd = fd;
	linkLayerData.numTransmissions = 0;
	
	unsigned char buf[255];

	unsigned char *setEnd = getSETDataPacket(buffer, length, _DISC);
	int setEndSize = length+5;

	stuffing(setEnd, setEndSize);

	int wrt = write(linkLayerData.fd, linkLayerData.frame, linkLayerData.sizeFrame);
	data_link_statistics.sentFrames++;

	//printf("%d bytes written\n", wrt);
	alarm(3);

	int state = 0;
	int sizeBuf = 0;
	unsigned char tmp[1];

	do
	{
		//Checking if it has send more than 4 times the packet
		if (linkLayerData.numTransmissions >= 4)
		{
			printf("Didn't get a response. BYE!\n");
			return -1;
		}

		int res = read(linkLayerData.fd, tmp, 1);

		//If the read is successful cancels the alarm. If not it continues trying to read
		if (res == -1)
			continue;

		//Should Have the stateMachine here to confirm when it reachs the end
		stateMachine(&state, tmp[0], buf, &sizeBuf);

		if (state == 5)
		{

			if (readResponse(linkLayerData.frame[2], buf[2]) != 0) //caso nao tenha recebido bem
			{
				wrt = write(linkLayerData.fd, linkLayerData.frame, linkLayerData.sizeFrame);
				
				alarm(3);
			}
			else
				break;
				
			state = 0;
			sizeBuf = 0;
		}


	} while (1);

	alarm(0);

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

	//Se for a trama inicial esta a espera de um UA
	if (originalFlag == _SET)
	{
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

	printf("\nTrying to send data packet again\n");

	if (linkLayerData.numTransmissions < 3)
	{

		write(linkLayerData.fd, linkLayerData.frame, linkLayerData.sizeFrame);
		data_link_statistics.sentFrames++;

		alarm(3);
	}

	data_link_statistics.noTimeouts++;
	linkLayerData.numTransmissions++;
}

unsigned char *getSETDataPacket(unsigned char *data, int sizeData , unsigned char CFlag)
{
    unsigned char *setBefore = (unsigned char *)malloc((sizeData + 5) * sizeof(unsigned char));

    setBefore[0] = FLAG;
    setBefore[1] = A;
    setBefore[2] = CFlag;
    setBefore[3] = setBefore[1] ^ setBefore[2]; //BCC1

    for (int i = 1; i <= sizeData; i++)
    {
        setBefore[i + 3] = data[i - 1];
    }

    setBefore[sizeData + 4] = FLAG;

    return setBefore;
}