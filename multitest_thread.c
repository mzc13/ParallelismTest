#include<sys/types.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#include "multitest.h"

char* RUNNINGMODE = "Threads";

typedef struct _ThreadSearchArgs {
	int* list;
	int remainingListSize;
	int partitionSize;
	int target;
	int threadNumber;
	int startIndex;
} ThreadSearchArgs;

void* _tSubSearch(void* _args) {
	ThreadSearchArgs* args = _args;
	int* list = args->list;
	int remainingListSize = args->remainingListSize;
	int partitionSize = args->partitionSize;
	int target = args->target;
	int* retValue = malloc(sizeof(int));
	int threadNumber = args->threadNumber;
	int startIndex = args->startIndex;

	partitionSize = (remainingListSize < partitionSize) ? remainingListSize : partitionSize;

	for (int i = 0; i < partitionSize; i++) {
		if (list[startIndex + i] == target) {
			*retValue = (threadNumber * partitionSize) + i;
			pthread_exit(retValue);
		}
	}

	*retValue = -1;
	pthread_exit(retValue);
}

int parallelSearch(int* list, int listSize, int partitionSize, int targetValue) {
	int numberOfPartitions = (listSize % partitionSize) ? (listSize / partitionSize) + 1 : (listSize / partitionSize);
	int targetIndex = -1;
	int** exitStatus = malloc(sizeof(int*) * numberOfPartitions);
	pthread_t threadArray[numberOfPartitions];
	ThreadSearchArgs** args = malloc(sizeof(ThreadSearchArgs*) * numberOfPartitions);
	for (int i = 0; i < numberOfPartitions; i++) {
		args[i] = malloc(sizeof(ThreadSearchArgs));
		args[i]->list = list;
		args[i]->partitionSize = partitionSize;
		args[i]->threadNumber = i;
		args[i]->remainingListSize = listSize;
		args[i]->startIndex = i * partitionSize;
		args[i]->target = targetValue;
		listSize -= partitionSize;
	}
	for (int i = 0; i < numberOfPartitions; i++) {
		if (pthread_create((threadArray + i), NULL, _tSubSearch, (void*) (args[i]))) {
			printf("Thread creation failed. Stopping Execution\n");
			exit(0);
		}
	}
	int indexFound = 0;
	for (int i = 0; i < numberOfPartitions; i++) {
		if (pthread_join(threadArray[i], (void**)(exitStatus))) {
			printf("Join Error. Stopping Execution\n");
			exit(0);
		}
		if(!indexFound && **(exitStatus) > -1){
			targetIndex = **(exitStatus);
			indexFound = 1;
		}
	}

	free(exitStatus);
	for (int i = 0; i < numberOfPartitions; i++)
		free(args[i]);
	free(args);
	return targetIndex;
}