/*Non-Canonical Input Processing*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03

unsigned char SET[5];

volatile int STOP=FALSE;

unsigned char buf[5];

int FLAG_ALARM = 1;
int count_ALARM = 0;
int fd,c, res;


void send() {
	res = write(fd,SET,5);
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
	SET[3]=SET[1]^SET[2];//BCC
	SET[4]=FLAG;

   
    struct termios oldtio,newtio;
    int i, sum = 0, speed = 0;
	(void) signal(SIGALRM, atende);

    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {//aqui a portas so é 4 em vez de 1 pq o pc pede o 4
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
	

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    fd = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */
  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH); //fazer fluish antes de mandar cenas

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");
 

  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
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
			FLAG_ALARM = 0;
		else
			continue;
		
		printf("%x\n",buf[n]);
		n++;
		} while (n<5);

   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

	close(fd);
 
    return 0;
}
