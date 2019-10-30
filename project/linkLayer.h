
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

