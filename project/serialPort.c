#include "serialPort.h"

struct termios oldtio, newtio;

#define RECEIVER 0
#define TRANSMITTER 1

int LLOPEN(int porta, int flag)
{
  if (flag == TRANSMITTER)
  {
    fd = open(porta, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
      perror(porta);
      exit(-1);
    }

    // Getting our file
    FILE *file = fopen(argv[2], "rb");

    if (file == NULL)
    {
      perror("Can't find such file");
      exit(-1);
    }
  }
  else if (flag == RECEIVER)
  {
    fd = open(porta, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
      perror(argv[1]);
      exit(-1);
    }
  }

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
    leitura do(s) prximo(s) caracter(es)
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

int LLCLOSE(int fd, struct termios *oldtio)
{

  if (tcsetattr(fd, TCSANOW, oldtio) == -1)
  {
    perror("tcsetattr");
    return -1;
  }

  close(fd);
  return 0;
}