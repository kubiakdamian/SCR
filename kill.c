#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* argv[]) {

	pid_t pid;

	int sig_number = 0;
	union sigval sig;

	if(argc != 4) {
		printf("Wrong data\n");
		return -1;
	}

	pid = atoi(argv[1]);
	sig_number = atoi(argv[2]);
	sig.sival_int = atoi(argv[3]);

	if(sigqueue(pid, sig_number, sig) == -1) { //signal to a process
        perror("sigqueue: ");
    }

    printf("Signal sent.\n");


	return 0;
}
