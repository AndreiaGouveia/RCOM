#define FLAG 0x7e
#define A 0x03
#define C_SET 0x40
#define C_SET_1 0x00
#define BCC A ^ C_SET

#define _SET 0x03
#define _DISC 0x0B
#define _UA 0x07
#define _RR 0x05
#define _REJ 0x01

#define STUFFING 0x7d
#define EXCLUSIVE_OR_STUFFING 0x20

#define RECEIVER 0
#define TRANSMITTER 1

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define SIZE_DATA 100

#define C_DATA 0x01
#define C_START 0x02
#define C_END 0x03