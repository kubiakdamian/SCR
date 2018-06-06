#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* argv[]) {

	pid_t pid;

	int sig_number = 0;
	union sigval sig;

	if(argc != 4) {
		printf("Niepoprawna ilosc argumentow\n Dodac argumenty pid, signal_id, data\n");
		// wszystko jako integery
		return -1;
	}

	pid = atoi(argv[1]);
	sig_number = atoi(argv[2]);
	sig.sival_int = atoi(argv[3]);

	//zamiana sygnalu na proces jak zwraca  -1 to blad
	if(sigqueue(pid, sig_number, sig) == -1) {
        perror("sigqueue: ");
    }
	//potwierdzenie wyslania
    printf("Sygnal %d z wartoscia %d zostal wyslany.\n", sig_number, sig.sival_int);


	return 0;
}
