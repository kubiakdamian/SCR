#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

void printSignalInfo(int value, int signalNumber){
	printf("Thread: %d %d %d \n", pthread_self(), value, signalNumber);
}

void setAvailableSignals(){
	sigset_t set;

	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK,&set,NULL);
}

void *firstSignal(void *arg){
	sigset_t set;
	siginfo_t signal_info_1;
	pthread_sigmask(SIG_BLOCK,&set,NULL);
	sigemptyset(&set);
	sigaddset(&set,SIGRTMIN);

	while(1){
		sigwaitinfo(&set,&signal_info_1);
		printSignalInfo(signal_info_1.si_value.sival_int, signal_info_1.si_signo);
	}

	return NULL;
}

void *secondSignal(void *arg){
	sigset_t set;
	siginfo_t signal_info_2;
	pthread_sigmask(SIG_BLOCK,&set,NULL);
	sigemptyset(&set);
	sigaddset(&set,SIGRTMIN+1);

	while(1){
		sigwaitinfo(&set,&signal_info_2);
		printSignalInfo(signal_info_2.si_value.sival_int, signal_info_2.si_signo);
	}

	return NULL;
}

void setThreads(){
	pthread_t thread_1;
	pthread_t thread_2;

	
	pthread_create(&thread_1,NULL,firstSignal,NULL);
	pthread_create(&thread_2,NULL,secondSignal,NULL);

	pthread_join(thread_1,NULL);
	pthread_join(thread_2,NULL);
}






int main(void) {
	setAvailableSignals();
	setThreads();

	return 0;
}
