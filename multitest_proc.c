#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#include "multitest.h"

char* RUNNINGMODE = "Processes";

int _pSubSearch(int* list, int remainingListSize, int partitionSize, int target) {
	partitionSize = (remainingListSize < partitionSize) ? remainingListSize : partitionSize;
	for (int i = 0; i < partitionSize; i++) {
		if (list[i] == target) {
			exit(i);
		}
	}
	exit(255);
}

int parallelSearch(int* list, int listSize, int partitionSize, int targetValue) {
	int numberOfPartitions = (listSize % partitionSize) ? (listSize / partitionSize) + 1 : (listSize / partitionSize);
	int targetIndex = -1;
	int exitStatus = 255;
	pid_t pidArray[numberOfPartitions];
	pid_t tempPid;
	for (int i = 0; i < numberOfPartitions; i++) {
		tempPid = fork();
		if (tempPid == 0) {
			_pSubSearch((list + (partitionSize * i)), listSize, partitionSize, targetValue);
		}
		else if (tempPid > 0) {
			pidArray[i] = tempPid;
		}
		else {
			printf("Fork failed. Stopping Execution.\n");
			exit(0);
		}
		listSize -= partitionSize;
	}
	int indexFound = 0;
	for (int i = 0; i < numberOfPartitions; i++) {
		if (waitpid(pidArray[i], &exitStatus, 0) < 0) {
			printf("Error while waiting for process. Stopping Execution.\n");
			exit(0);
		}
		if (!indexFound && WEXITSTATUS(exitStatus) < 251) {
			targetIndex = (partitionSize * i) + WEXITSTATUS(exitStatus);
			indexFound = 1;
		}
	}
	return targetIndex;
}