#include "utilities.h"

void progressBar(float percentageReceived){

	printf("\rCompleted: %f[", percentageReceived);

	for(int i = 0; i < percentageReceived; i += 5){
		printf("=");
	}
	printf("]");

	fflush (stdout);
}

void printStatistics(){

    printf("================================\n");
    printf("STATISTICS\n");
    printf("================================\n");
    printf("No of Frames Sent: %d\n", data_link_statistics.sentFrames);
    printf("No of Frames Received: %d\n", data_link_statistics.receivedFrames);
    printf("No of TimeOuts: %d\n", data_link_statistics.noTimeouts);
    printf("No of RR: %d\n", data_link_statistics.noRR);
    printf("No of REJ: %d\n", data_link_statistics.noREJ);
    printf("\n");
}

void initStatistics(){

    data_link_statistics.noREJ=0;
    data_link_statistics.noRR=0;
    data_link_statistics.noTimeouts=0;
    data_link_statistics.receivedFrames=0;
    data_link_statistics.sentFrames=0;
    data_link_statistics.timeSpent=0;
}