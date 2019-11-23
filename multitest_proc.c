#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#include "multitest.h"

char* RUNNINGMODE = "Processes";

// Assign a thread to search. Exits with index of target relative to where this process started searching, or 255 if not found.
int _pSubSearch(int* list, int remainingListSize, int partitionSize, int target) {
	// Ensures the program doesn't access any values outside of the list
	partitionSize = (remainingListSize < partitionSize) ? remainingListSize : partitionSize;

	for (int i = 0; i < partitionSize; i++) {
		if (list[i] == target) {
			exit(i);
		}
	}
	exit(255);
}

// Use processes to search a list
int parallelSearch(int* list, int listSize, int partitionSize, int targetValue) {
	// Calculate number of partitions/processes required
	int numberOfPartitions = (listSize % partitionSize) ? (listSize / partitionSize) + 1 : (listSize / partitionSize);

	// Initializing variables to keep track processes
	int exitStatus = 255;
	pid_t pidArray[numberOfPartitions];
	pid_t tempPid;

	// Fork processes to search and stop execution if fork fails
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

	// Get exit statuses and stop execution if wait fails
	int indexFound = 0;
	int targetIndex = -1;
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