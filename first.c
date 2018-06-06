#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

void printSignalInfo(int value, int signalNumber){
	printf("Thread: %d %d %d \n", pthread_self(), value, signalNumber);
}

void setAvailableSignals(){
	sigset_t set;

	if(sigfillset(&set) == -1){ //Fill with all signals
		return -1;
	} 
	pthread_sigmask(SIG_BLOCK,&set,NULL); //mask with set union, to fetch set with proper thread
}

void *firstSignal(void *arg){
	sigset_t set;
	siginfo_t firstInfo;
	pthread_sigmask(SIG_BLOCK,&set,NULL);
	sigemptyset(&set); //initializes set
	sigaddset(&set,SIGRTMIN); // range of supported real-time signals SIGRTMIN to SIGRTMAX

	while(1){
		sigwaitinfo(&set,&firstInfo);
		printSignalInfo(firstInfo.si_value.sival_int, firstInfo.si_signo);
	}

	return NULL;
}

void *secondSignal(void *arg){
	sigset_t set;
	siginfo_t secondInfo;
	pthread_sigmask(SIG_BLOCK,&set,NULL);
	sigemptyset(&set);
	sigaddset(&set,SIGRTMIN+1);

	while(1){
		sigwaitinfo(&set,&secondInfo);
		printSignalInfo(secondInfo.si_value.sival_int, secondInfo.si_signo);
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
