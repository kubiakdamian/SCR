#include <sys/neutrino.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define TABSIZE 8

int getRandomNumber(int min, int max){
        return (rand() % (max + 1 - min)) + min;
}

void raiseToSquare(int numbers[], int size){
        for(int i = 0; i < size; i++){
             numbers[i] *= numbers[i];   
        } 
}

int main(int argc, char *argv[])
{
        int connectionId, receiveChannelId, rcvid;
        char messageName[8], messageReceived[8];
        int *numbersHandler;
        int sharedMemmory;
        int memorySize = TABSIZE * sizeof(int);
        pid_t processId = getpid();
        pid_t pid;      

        srand(time(NULL)); 

        printf("Server started\n");

        if((receiveChannelId = ChannelCreate(0)) == -1) { // create a channel that can be used to receive messages 
                perror("Channel creation: ");
                return EXIT_FAILURE;
        }

        if((pid = fork()) == -1) {
                perror("Fork: ");
                return EXIT_FAILURE;
        }

        if(pid == 0) { 
                printf("Client started\n");

                if((connectionId = ConnectAttach(0, processId, receiveChannelId, 0, 0)) == -1) { // connect receiveChannelId to pid
                        perror("Connection: ");
                        return EXIT_FAILURE;
                }

                strcpy(messageName, "message");

                if((sharedMemmory = shm_open(messageName, O_RDWR | O_CREAT, 0777)) == -1) { //file descriptor.This file descriptor is used by other functions to refer to the shared memory object
                        perror("Opening memory object: ");
                        return EXIT_FAILURE;
                }

                if(ftruncate(sharedMemmory, memorySize) != 0 ) {
                        perror("truncation to a size: ");
                        return EXIT_FAILURE;
                }

                if((numbersHandler = (int*)mmap(NULL, memorySize, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemmory, 0 )) == MAP_FAILED) { //MAP_SHARED - data will work in the same memory area instead of creating new ones
                        perror("Mapping: ");
                        return EXIT_FAILURE;
                }       

                printf("Generated data: \n");
                for(int i = 0; i < TABSIZE; i++){
                        *(numbersHandler+i) = getRandomNumber(0, 10);
                        printf("%d\n", *(numbersHandler+i));
                }

                if((MsgSend(connectionId, &messageName, sizeof(messageName), &messageReceived, sizeof(messageReceived))) == -1) { //send a message to a process's channel
                        perror("Message sending: "); 
                        return EXIT_FAILURE;
                }

                printf("Data received \n");

                for(int i = 0; i < TABSIZE; i++){
                        printf("%d\n", *(numbersHandler+i));
                }

                if(ConnectDetach(connectionId) == -1){
                        perror("Detaching connection: ");
                        return EXIT_FAILURE;
                }

                shm_unlink(messageName); //After unlink I can't access the object
                printf("Client closed\n");
        }

        else { 
                if((rcvid = MsgReceive(receiveChannelId, &messageName, sizeof(messageName), NULL)) == -1){
                        perror("Message receiving: ");
                        return EXIT_FAILURE;
                }

                if((sharedMemmory = shm_open(messageName, O_RDWR | O_CREAT, 0777)) == -1) {
                        perror("Opening memory object: ");
                        return EXIT_FAILURE;
                }

                if((numbersHandler = (int*)mmap(NULL, memorySize, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemmory, 0 )) == MAP_FAILED) {
                        perror("Mapping: ");
                        return EXIT_FAILURE;
                }

                printf("Raising to square\n");
                raiseToSquare(numbersHandler, TABSIZE);


                if(munmap(numbersHandler, memorySize) == -1) {
                        perror("Unmapping: ");
                        return EXIT_FAILURE;
                }

                close(sharedMemmory);

                if((MsgReply(rcvid, NULL, &messageName, sizeof(messageName))) == -1){
                        perror("Replying: ");
                        return EXIT_FAILURE;
                }

                waitpid(pid, NULL, 1); 

                if(ChannelDestroy(receiveChannelId) == -1) {
                        perror("Destroying: ");
                        return EXIT_FAILURE;
                }

                printf("Server closed\n");
        }

        return 0;
}