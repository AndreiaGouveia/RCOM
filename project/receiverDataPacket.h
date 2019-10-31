#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "linkLayerReceiver.h"

/**
 * Gets file info (name and size)
 *
 * @param initialDataPacket data packet to be analysed
 * @param sizeInitialDataPacket size of the inital packet
 * @param nameOfFile array with the name of the file
 * @param sizeOfName size of the name of the file
 * @param sizeOfFile size of file
 *
 * @return Returns 0 if everything went as expected, other number if an error occured
 *
*/
int getInfoFile(unsigned char *initialDataPacket, int sizeInitialDataPacket, char ** nameOfFile, int * sizeOfName, int * sizeOfFile);

/**
 * Takes the data information and puts it in our final file
 *
 * @param dataPacket data packet to be analysed
 * @param sizeDataPacket size of the data packet
 * @param fullFile array that will contain the full file
 * @param beginPosition position to start writing on fullFile
 *
 * @return Returns 0 if everything went as expected, other number if an error occured
 *
*/
int getData(unsigned char * dataPacket, int sizeDataPacket, unsigned char ** fullFile, int beginPosition);

/**
 * Creates a file with the information received (in our fullFile array)
 *
 * @param nameOfFile name of the file to be created
 * @param sizeOfFile size of the file to be created
 * @param fullFile array that contains the full file
 *
 * @return void
 *
*/
void createFile(unsigned char * nameOfFile, int sizeOfFile, unsigned char * fullFile);

/**
 * Prints the progress bar
 *
 * @param percentageReceived current percentage of the progress bar
 *
 * @return void
 *
*/
void progressBar(float percentageReceived);