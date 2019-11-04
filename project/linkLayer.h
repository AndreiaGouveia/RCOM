
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

enum Status { TRANSMITTER, RECEIVER};

/**
 * @param porta number of pipe
 * @param flag if it is a transmiter or a receptor
 * 
 * @param file descriptor of pipe in case of success, returns negative value otherwise
*/
int LLOPEN( int porta, enum Status status);

/**
 * @param fd file descriptor of pipe
 * 
 * @return returns a positive number in case of success, negative otherwise
*/
int LLCLOSE(int fd);

void stateMachine(int *state, unsigned char byte_received, unsigned char SET[], int *sizeSET);
