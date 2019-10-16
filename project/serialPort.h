
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

#define BAUDRATE B38400


int LLOPEN( int fd, struct termios * newtio, struct termios * oldtio);

int LLCLOSE(int fd, struct termios * oldtio);