#include "utilities.h"

void progressBar(float percentageReceived)
{

  printf("\rCompleted: %f[", percentageReceived);

  for (int i = 0; i < percentageReceived; i += 5)
  {
    printf("=");
  }
  printf("]");

  fflush(stdout);
}

void printStatistics()
{

  printf("================================\n");
  printf("STATISTICS\n");
  printf("================================\n");
  printf("Transmission Velocity: %d\n", BAUDRATE);
  printf("Number of seconds to timeout: %d\n", SECONDS_WAIT_TIMEOUT);
  printf("Number of tries: %d\n", N_TRIES_TIMEOUT);
  printf("\n");
  printf("No of Frames Sent: %d\n", data_link_statistics.sentFrames);
  printf("No of Frames Received: %d\n", data_link_statistics.receivedFrames);
  printf("No of TimeOuts: %d\n", data_link_statistics.noTimeouts);
  printf("No of RR: %d\n", data_link_statistics.noRR);
  printf("No of REJ: %d\n", data_link_statistics.noREJ);
  printf("Time Spent: %f s\n", data_link_statistics.timeSpent);
  printf("\n");
}

void initStatistics()
{

  data_link_statistics.noREJ = 0;
  data_link_statistics.noRR = 0;
  data_link_statistics.noTimeouts = 0;
  data_link_statistics.receivedFrames = 0;
  data_link_statistics.sentFrames = 0;
  data_link_statistics.timeSpent = 0;
}

unsigned char *messUpBCC2(unsigned char *packet, int sizePacket)
{
  unsigned char *copia = (unsigned char *)malloc(sizePacket);
  memcpy(copia, packet, sizePacket);

  int percentageOfErrorInEachTrama = (rand() % 100) + 1;
  if (percentageOfErrorInEachTrama <= bcc2ErrorPercentage)
  {

    int i = (rand() % (sizePacket - 5)) + 4;
    unsigned char randomLetter = (unsigned char)('A' + (rand() % 26));
    copia[i] = randomLetter;
  }

  return copia;
}

unsigned char *messUpBCC1(unsigned char *packet, int sizePacket)
{
  unsigned char *copia = (unsigned char *)malloc(sizePacket);
  memcpy(copia, packet, sizePacket);
  
  int percentageOfErrorInEachTrama = (rand() % 100) + 1;
  if (percentageOfErrorInEachTrama <= bcc1ErrorPercentage)
  {
    int i = (rand() % 3) + 1;
    unsigned char randomLetter = (unsigned char)('A' + (rand() % 26));
    copia[i] = randomLetter;
  }

  return copia;
}