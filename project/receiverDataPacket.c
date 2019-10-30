#include "receiverDataPacket.h"

int getInfoFile(unsigned char *initialDataPacket, int sizeInitialDataPacket, char **nameOfFile, int *sizeOfName, int *sizeOfFile)
{

	for (int i = 5; i < sizeInitialDataPacket; i++)
	{

		if (initialDataPacket[i] == 0x00)
		{

			i++;
			int sizeOfValue = initialDataPacket[i];

			int offset = i + 1;

			for (int j = offset; j < (sizeOfValue + offset) && j < sizeInitialDataPacket; j++)
			{

				(*sizeOfFile) = (*sizeOfFile) << 8 * (j - offset);
				(*sizeOfFile) |= initialDataPacket[j];
				i++;
			}
		}
		else if (initialDataPacket[i] == 0x01)
		{

			i++;
			*sizeOfName = initialDataPacket[i];

			int offset = i + 1;

			(*nameOfFile) = malloc(sizeof(unsigned char) * (*sizeOfName));

			for (int j = offset; j < ((*sizeOfName) + offset) && j < sizeInitialDataPacket; j++)
			{

				(*nameOfFile)[j - offset] = initialDataPacket[j];
				i++;
			}
		}
	}

	return 0;
}

int getData(unsigned char *dataPacket, int sizeDataPacket, unsigned char **fullFile, int beginPosition)
{
	int numOctets = 256*dataPacket[6] + dataPacket[7];

	for (int i = 0; i < numOctets; i++)
	{
		(*fullFile)[i + beginPosition] = dataPacket[i+8];
	}

	return numOctets;
}
