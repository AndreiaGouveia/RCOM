
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
