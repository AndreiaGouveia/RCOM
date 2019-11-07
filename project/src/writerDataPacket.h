#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "utilities.h"
#include "linkLayerWriter.h"

enum WhichControl { Begin,
                    End}; //enum that tells if we are dealing with the initial or final message

/**
 * gets the full control data packet to be sent
 *
 * @param fileName name of the file to be sent
 * @param cf know if it is begin or end
 * @param fileSize size of file
 * @param initialSet new packet to be sent
 * @param sizeInitialSet size of new packet
 *
 * @return Returns 0 if everything went as expected, other number if an error occured
 *
*/
int getControlDataPacket(char fileName[], enum WhichControl cf, int fileSize, unsigned char **initialSet, int *sizeInitialSet);

/**
 * Reads file
 *
 * @param file file descriptor
 * @param size size of file to be sent
 * @param filename name of file to be sent
 *
 * @return void
 *
*/
unsigned char *readFile(FILE *file, size_t *size, unsigned char *fileName);

/**
 * Gets the full data packet
 *
 * @param data data too be sent
 * @param sizeData suze of data
 * @param fullData full data packet
 * @param sizefullData szze of final data packet
 * @param nSeq number of the packet 
 *
 * @return Returns 0 if everything went as expected, other number if an error occured
 *
*/
int getFullDataPacket(unsigned char *data, int sizeData, unsigned char **fullData, int *sizefullData, int nSeq);

/**
 * Prints the progress bar
 *
 * @param fd port to send packet
 * @param fileSize size of file  
 * @param fullData packet to be sent 
 *
 * @return void
 *
*/
void sendFileData(int fd, int fileSize, unsigned char * fullData);

/**
 * Sends crontol data packet
 *
 * @param fd port to send packet
 * @param cf know if it is begin or end
 * @param fileName name of file 
 * @param file file descriptor 
 * @param fileSize size of file 
 *
 * @return void
 *
*/
void sendControlDataPacket(int fd, enum WhichControl cf, char * fileName, int fileSize);

/**
 * Send UA packet
 *
 * @param fd  port to send packet
 *
 * @return void
 *
*/
void sendUA(int fd);
