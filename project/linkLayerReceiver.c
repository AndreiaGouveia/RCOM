#include "linkLayerReceiver.h"

int expectedNumSeq = 0;

//Receiver functions

int LLREAD(int fd, unsigned char **dataPacket)
{

    int sizeDataPacket = 0;

    do
    {
        unsigned char *internalDataPacket;

        readInfoDataPacket(fd, &internalDataPacket, &sizeDataPacket);

        //If we received a duplicated datapacket we ignore it and read what is next in the pipe
        if (internalDataPacket[5] < expectedNumSeq)
        {
            sendResponse(fd, internalDataPacket[2]);
            continue;
        }

        //Checks if the sequence number is the one we want
        if (internalDataPacket[5] == expectedNumSeq)
        {
            if (checkErrors(internalDataPacket, sizeDataPacket, Data))
            {

                expectedNumSeq++;

                sendResponse(fd, internalDataPacket[2]);
                //printf("Received the info correctly!\n");

                *dataPacket = internalDataPacket;

                break;
            }
        }

        //Arives here if the bcc2 is not correct or the trama number is not correct
        sendResponse(fd, _REJ);
        //printf("Something went wrong!\n");

        free(internalDataPacket);

    } while (TRUE);

    return sizeDataPacket;
}

void recivingInformationDataPacket(int fd, unsigned char **dataPacket, int *sizeDataPacket, enum TypeDataPacketI typeDataPacketI)
{

    do
    {
        readInfoDataPacket(fd, dataPacket, sizeDataPacket);

        if (checkErrors(*dataPacket, *sizeDataPacket, typeDataPacketI))
        {
            sendResponse(fd, (*dataPacket)[2]);
            //printf("Received the info correctly!\n");

            break;
        }

        sendResponse(fd, _REJ);
        //printf("Something went wrong!\n");

    } while (TRUE);
}

int readInfoDataPacket(int fd, unsigned char **dataPacket, int *sizeDataPacket)
{
    int res = 0;
    unsigned char buf[5];

    unsigned char SET[255];
    int n = 0;
    int sizeSET = 0;

    while (1)
    {
        res = read(fd, buf, 1);

        stateMachine(&n, buf[0], SET, &sizeSET);

        if (n == 5)
        {
            destuffing(SET, sizeSET, dataPacket, sizeDataPacket);

            break;
        }
    }

    return 0;
}

void sendResponse(int fd, unsigned char controlBit)
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
        sendDataPacket[1] = A_1;
        break;

    case _UA:
        sendDataPacket[2] = _UA;
        break;

    case C_SET:
        sendDataPacket[2] = _RR;

        sendDataPacket[2] |= controlBit << 1;

        break;

    case C_SET_1:
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

    write(fd, sendDataPacket, 5);
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

void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeSET)
{

    switch (*state)
    {

    case 0:
        if (byte_received == FLAG)
        {
            SET[*sizeSET] = byte_received;
            *state = 1;
        }
        break;
    case 1:
        if (byte_received == A)
        {
            SET[*sizeSET] = byte_received;
            *state = 2;
        }
        else if (byte_received != FLAG)
            *state = 0;
        break;
    case 2:
        if (byte_received == C_SET || byte_received == _SET || byte_received == _DISC || byte_received == _UA)
        {
            SET[*sizeSET] = byte_received;
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
        if (byte_received == SET[*sizeSET - 1] ^ SET[*sizeSET - 2])
        {
            SET[*sizeSET] = byte_received;
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
        SET[*sizeSET] = byte_received;

        if (byte_received == FLAG)
        {
            *state = 5;
        }
        else
            *state = 4;
        break;
    }

    (*sizeSET)++;
}

int checkErrors(unsigned char SET[], int sizeSET, enum TypeDataPacketI typeDataPacketI)
{

    if (!checkBCC2(SET, sizeSET))
        return FALSE;

    switch (typeDataPacketI)
    {
    case Start:
        if (SET[4] == C_START)
            return TRUE;
        break;

    case Data:
        if (SET[4] == C_DATA)
            return TRUE;
        break;

    case End_:
        if (SET[4] == C_END)
            return TRUE;
        break;
    }

    return FALSE;
}

int checkBCC2(unsigned char SET[], int sizeSET)
{

    unsigned char BCC2 = 0x00;

    for (int i = 4; i < sizeSET - 2; i++)
    {

        BCC2 ^= SET[i];
    }

    if (BCC2 == SET[sizeSET - 2])
        return TRUE;
    else
        return FALSE;
}

//END RECEIVER FUNCTIONS =============================