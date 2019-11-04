
#include <stdio.h>

#define FLAG 0x7e
#define A 0x03
#define A_1 0x03
#define C_SET 0x40
#define C_SET_1 0x00
#define BCC A ^ C_SET

#define _SET 0x03 //set up
#define _DISC 0x0B //disconnect
#define _UA 0x07 //unnumbered acknowledgment
#define _RR 0x05 //receiver ready
#define _REJ 0x01 //reject

#define STUFFING 0x7d
#define EXCLUSIVE_OR_STUFFING 0x20

#define BAUDRATE B38400 /*transmission velocity*/
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define SIZE_DATA 100 /* size of data in each transaction*/

//Control field of control packets
#define C_DATA 0x01 /*data transmission*/
#define C_START 0x02 /*beginning of transmission*/
#define C_END 0x03 /*end of transmission*/

//serial ports
#define COM0 "/dev/ttyS0"
#define COM1 "/dev/ttyS1"
#define COM2 "/dev/ttyS2"
#define COM3 "/dev/ttyS3"
#define COM4 "/dev/ttyS4"
#define COM5 "/dev/ttyS5"
#define COM6 "/dev/ttyS6"
#define COM7 "/dev/ttyS7"


typedef struct {
	int sentFrames;
	int receivedFrames;
    int noTimeouts;
	int noRR;
	int noREJ;
  double timeSpent;
} Statistics;

Statistics data_link_statistics;

/**
 * Prints the progress bar
 *
 * @param percentageReceived current percentage of the progress bar
 *
*/
void progressBar(float percentageReceived);