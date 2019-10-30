
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include "flags.h"

typedef struct
{
    int fd;
    unsigned int numTransmissions; /*Número de tentativas em caso de falha*/
    unsigned char * frame;          /*Trama*/
    int sizeFrame;
} linkLayer;

linkLayer linkLayerData;

enum TypeDataPacketI { Start,
                    End_, Data};


/**
 * @param porta number of pipe
 * @param flag if it is a transmiter or a receptor
 * 
 * @param file descriptor of pipe in case of success, returns negative value otherwise
*/
int LLOPEN( int porta, int flag);

/**
 * @param fd file descriptor of pipe
 * 
 * @return returns a positive number in case of success, negative otherwise
*/
int LLCLOSE(int fd);

/**
 * Transmites an array of characters
 * 
 * @param fd fd File Discriptor where Data Packets should be wrote and read
 * @param buffer array of characters to be transmitted
 * @param lenght size of buffer
 * 
 * @return Returns the number of written characters, in case of error returns a negative number
 * 
*/
int LLWRITE(int fd , unsigned char *buffer, int length);

/**
* Stuffs a Data Packet
*
* Array after stuffing is saved in the global variable linkLayerData of type linkLayer
*
* @param beforeStuffing Pointer to the data packet array before stuffing
* @param sizeBeforeStuffing Size of the data packet array before stuffing
*
*/
int stuffing(unsigned char *beforeStuffing, int sizeBeforeStuffing);

/**
 * Checks if the response data packet is of acceptance
 * 
 * @param originalFlag control Bit sent
 * @param cFlag control Bit received
 * 
 * @return Returns 0 if the response data packet is of acceptance, 1 otherwise
*/
int readResponse(unsigned char originalFlag, unsigned char cFlag);

void atende();

/**
 * Handles the reception of and Information Data Packet (I)
 * 
 * @param dataPacket Pointer to an array that will hold the Data Packet Read (this function allocates the necessary memory)
 * @param sizeDataPacket Pointer to an int that will hold the size of the dataPacket array
 * 
*/
void recivingInformationDataPacket(unsigned char **dataPacket, int *sizeDataPacket, enum TypeDataPacketI typeDataPacketI);

/**
 * Reads a Data Packet
 * 
 * @param fd File Discriptor where Data Packet should be read
 * @param dataPacket Pointer to an array that will hold the Data Packet Read (this function allocates the necessary memory)
 * 
 * @return Return the number of bits read (size of dataPacket)
*/
int LLREAD(int fd, unsigned char **dataPacket);

/**
* Sends the Response to a Information Data Packet (I)
*
* @param controlBit Header's Control Bit of the Information Data Packet (I)
*
*/
void sendResponse(unsigned char controlBit);

/**
* Destuffs a Data Packet
*
* @param SET Pointer to the data packet array
* @param sizeSET Size of the data packet array
* @param afterDestuffing Pointer to an array that will hold the data packet after destuffing (this function allocates the necessary memory)
* @param sizeAfterDestuffing Size of the array afterDestuffing
*
*/
int destuffing(unsigned char * SET, int sizeSET, unsigned char * * afterDestuffing, int * sizeAfterDestuffing);

/**
* State machine of an incoming data packet
* If state is 5 the data packet is completed
*
* @param state pointer to an int that represents the state in which an incoming data packet is at
* @param byte_received the byte received
* @param SET Pointer to the information data packet (I) array (it is updated if necessary)
* @param sizeSET Size of the information data packet array so far (it is incremented if necessary)
* 
*/
void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeSET);

/**
* Checks if the BBC2 in an information data packet (I) is correct
*
* @param SET Pointer to the information data packet (I) array
* @param sizeSET Size of the information data packet array
* 
* @return Return 1 when the data packet has no detected errors and 0 otherwise
*/
int checkBCC2(unsigned char SET[], int sizeSET);

/**
* Checks for errors in an information data packet (I)
*
* @param SET Pointer to the information data packet (I) array
* @param sizeSET Size of the information data packet array
* @param typeDataPacketI Type of data packet expected to received
* 
* @return Return 1 when the data packet has no detected errors and 0 otherwise
*/
int checkErrors(unsigned char SET[], int sizeSET, enum TypeDataPacketI typeDataPacketI);
