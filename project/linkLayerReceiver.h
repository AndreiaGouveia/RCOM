#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include "utilities.h"

#include "linkLayer.h"

enum TypeDataPacketI { Start,
                    End_, Data};

extern Statistics data_link_statistics;

/**
 * Handles the reception of and Information Data Packet (I)
 * 
 * @param dataPacket Pointer to an array that will hold the Data Packet Read (this function allocates the necessary memory)
 * @param sizeDataPacket Pointer to an int that will hold the size of the dataPacket array
 * 
*/
void recivingInformationDataPacket(int fd, unsigned char **dataPacket, int *sizeDataPacket, enum TypeDataPacketI typeDataPacketI);

/**
 * Reads a Data Packet
 * 
 * @param fd File Discriptor where Data Packet should be read
 * @param dataPacket Pointer to an array that will hold the Data Packet Read (this function allocates the necessary memory)
 * 
 * @return Return the number of bits read (size of dataPacket)
*/
int LLREAD(int fd, unsigned char **dataPacket);


int readInfoDataPacket(int fd, unsigned char **dataPacket, int *sizeDataPacket);

/**
* Sends the Response to a Information Data Packet (I)
*
* @param controlBit Header's Control Bit of the Information Data Packet (I)
*
*/
void sendResponse(int fd, unsigned char controlBit);

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
