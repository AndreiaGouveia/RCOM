/*Non-Canonical Input Processing*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "serialPort.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03

unsigned char SET[255];

volatile int STOP=FALSE;

unsigned char buf[255];

int FLAG_ALARM = 1;
int count_ALARM = 0;
int fd,c, res;

void getBCC2(unsigned char * data, int sizeData, unsigned char BBC2){

	BBC2 = 0x00;
	for(int i = 0; i < sizeData; i++){
		BBC2 = BBC2 ^ data[i];
	}

}


void send() {
	res = write(fd,SET,6);
	printf("%d bytes written\n", res);
}

void atende() {

printf("atendeu\n");

if(FLAG_ALARM == 1 && count_ALARM <= 3)
	{

		if (count_ALARM < 3)
		{send();
		alarm(3);}
		count_ALARM++;
	}

}

int main(int argc, char** argv)
{

//CRIACAO DO SET
	SET[0]=FLAG;
	SET[1]=A;
	SET[2]=C_SET;
	SET[3]=SET[1]^SET[2];//BCC1
	getBCC2(NULL, 0, SET[4]);
	SET[5]=FLAG;


    struct termios oldtio,newtio;
    int i, sum = 0, speed = 0;
	(void) signal(SIGALRM, atende);


    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0)  && (strcmp("/dev/ttyS2", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
	
  	fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
      if (fd < 0)
	{
		perror(argv[1]);
		exit(-1);
	}

	OpenSerialPort(fd, &newtio, &oldtio);
  


  /*
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
    o indicado no gui�o
  */

		send();
		alarm (3);
		//write

		int n=0;

		//read
		do{

		if(count_ALARM >= 4){
			printf("Didn't get a response. BYE!\n");
			break;
		}

		res=read(fd,&buf[n],1);

		if(res != -1)
			{
			FLAG_ALARM = 0;
			//signal(SIGALRM, SIG_IGN);
			}
		else
			continue;

		printf("%x\n",buf[n]);

		if(n != 0){
			if(buf[n] == FLAG)
				break;
		}
		n++;

	} while (1);

    CloseSerialPort(fd, &oldtio);

    return 0;
}
