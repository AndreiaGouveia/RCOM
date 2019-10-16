#include "serialPort.h"

int OpenSerialPort(int fd, struct termios *newtio, struct termios *oldtio)
{
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  if (tcgetattr(fd, oldtio) == -1)
  { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(newtio, sizeof(struct termios));
  newtio->c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio->c_iflag = IGNPAR;
  newtio->c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio->c_lflag = 0;

  newtio->c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio->c_cc[VMIN] = 1;  /* blocking read until 5 chars received */
                           /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH); //fazer fluish antes de mandar cenas

  if (tcsetattr(fd, TCSANOW, newtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  return 0;
}

int CloseSerialPort(int fd, struct termios *oldtio)
{

  if (tcsetattr(fd, TCSANOW, oldtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
}