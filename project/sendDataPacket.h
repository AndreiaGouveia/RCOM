#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03

void LLWRITE(int fd, unsigned char *data, int sizeData);
void atende();