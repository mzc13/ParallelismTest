#include<sys/time.h>
#include<limits.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include "multitest.h"

// Comment out this line for a random seed on every execution
//#define time(NULL) 0

typedef struct _TestResults {
	long mean;
	long stdDev;
	long min;
	long max;
} TestResults;

int* generateRandomList(int size) {
	int* list = malloc(sizeof(int) * size);
	for (int i = 0; i < size; i++) {
		list[i] = i;
	}
	int randIndex0 = rand() % size;
	int randIndex1 = rand() % size;
	int temp = -1;
	for (int i = 0; i < (size * 0.8); i++) {
		temp = list[randIndex0];
		list[randIndex0] = list[randIndex1];
		list[randIndex1] = temp;

		randIndex0 = rand() % size;
		randIndex1 = rand() % size;
	}

	return list;
}

int generateNewList(int* list, int size, int index) {
	int randIndex = rand() % size;
	int temp = list[index];
	list[index] = list[randIndex];
	list[randIndex] = temp;
	return 0;
}

long getMean(long times[], int size) {
	long total = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		total += times[i];
	}
	return total / size;
}

long getStdDev(long times[], int size, long mean) {
	double numeratorSum = 0;
	for (int i = 0; i < size; i++) {
		numeratorSum += pow(times[i] - mean, 2);
	}
	return sqrt(numeratorSum / (size - 1));
}

long testTime(char testIdentifier, int* list, int listSize, int partitionSize, int target, int iteration, int* index) {
	struct timeval start, end;
	gettimeofday(&start, NULL);
	*index = dummySearch(list, listSize, partitionSize, target);
	gettimeofday(&end, NULL);
	int seconds = end.tv_sec - start.tv_sec;
	int microSeconds = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;
	// printf("Test %c-%d | Found %d at : %d\n", testIdentifier, iteration, target, *index);
	return microSeconds;
}

TestResults constantPartitionSizeTest(char testIndicator, int* list, int listSize, int numberOfRuns) {
	int partitionSize = 250;
	int target = listSize / 2;
	int targetIndex = -1;
	long times[numberOfRuns];
	long min = __LONG_MAX__;
	long max = -1;
	for (int i = 0; i < numberOfRuns; i++) {
		times[i] = testTime(testIndicator, list, listSize, partitionSize, target, i, &targetIndex);
		//printf("Prev Value at %d : %d | \n", targetIndex, list[targetIndex]);
		generateNewList(list, listSize, targetIndex);
		//printf("New Value at %d : %d\n", index, list[index]);
		min = (times[i] < min) ? times[i] : min;
		max = (times[i] > max) ? times[i] : max;
	}
	TestResults retVal;
	retVal.mean = getMean(times, numberOfRuns);
	retVal.stdDev = getStdDev(times, numberOfRuns, retVal.mean);
	retVal.min = min;
	retVal.max = max;
	return retVal;
}

TestResults constantListSizeTest(char testIndicator, int* list, int partitionSize, int numberOfRuns) {
	int listSize = 5000;
	int target = listSize / 2;
	int targetIndex = -1;
	long times[numberOfRuns];
	long min = __LONG_MAX__;
	long max = -1;
	for (int i = 0; i < numberOfRuns; i++) {
		times[i] = testTime(testIndicator, list, listSize, partitionSize, target, i, &targetIndex);
		//printf("Prev Value at %d : %d | \n", targetIndex, list[targetIndex]);
		generateNewList(list, listSize, targetIndex);
		//printf("New Value at %d : %d\n", index, list[index]);
		min = (times[i] < min) ? times[i] : min;
		max = (times[i] > max) ? times[i] : max;
	}
	TestResults retVal;
	retVal.mean = getMean(times, numberOfRuns);
	retVal.stdDev = getStdDev(times, numberOfRuns, retVal.mean);
	retVal.min = min;
	retVal.max = max;
	return retVal;
}

int main() {
	srand(time(NULL));
	int batchSize = 100;
	int testingListSize = 2000;
	int overflow = 1;
	int numberOfExecutions = 0;
	int* list = generateRandomList(testingListSize);
	for(int i = 20; i < 251; i++){
		// int numberOfExecutions = (testingListSize % i) ? (testingListSize/i) + 1 : (testingListSize/i);
		// if(testingListSize % i == 0){
		// 	TestResults temp = constantListSizeTest('Z', list, i, batchSize);
		// 	printf("%d, %ld, %d\n", (int)(testingListSize/i), temp.mean, i);
		// 	overflow = 1;
		// }else if(overflow){
		// 	TestResults temp = constantListSizeTest('Z', list, i, batchSize);
		// 	printf("%d, %ld, %d\n", (int)(testingListSize/i) + 1, temp.mean, i);
		// 	overflow = 0;
		// }
		int numberOfPartitions = (testingListSize % i) ? (testingListSize / i) + 1 : (testingListSize / i);
		TestResults temp = constantListSizeTest('Z', list, i, batchSize);
		printf("%d, %ld, %d\n", i, temp.mean, numberOfPartitions);
	}
/*
	int* listOfSize250 = generateRandomList(250);
	TestResults a = constantPartitionSizeTest('A', listOfSize250, 250, batchSize);
	free(listOfSize250); 
	listOfSize250 = NULL;

	int* listOfSize1000 = generateRandomList(1000);
	TestResults c = constantPartitionSizeTest('C', listOfSize1000, 1000, batchSize);
	free(listOfSize1000); 
	listOfSize1000 = NULL;

	int* listOfSize2000 = generateRandomList(2000);
	TestResults d = constantPartitionSizeTest('D', listOfSize2000, 2000, batchSize);
	free(listOfSize2000); 
	listOfSize2000 = NULL;

	int* listOfSize4000 = generateRandomList(4000);
	TestResults e = constantPartitionSizeTest('E', listOfSize4000, 4000, batchSize);
	free(listOfSize4000); 
	listOfSize4000 = NULL;

    int* listOfSize6000 = generateRandomList(6000);
    TestResults f = constantPartitionSizeTest('F', listOfSize6000, 6000, batchSize);
    free(listOfSize6000);
    listOfSize6000 = NULL;

	int* listOfSize8000 = generateRandomList(8000);
	TestResults g = constantPartitionSizeTest('G', listOfSize8000, 8000, batchSize);
	free(listOfSize8000); 
	listOfSize8000 = NULL;

	int* listOfSize500 = generateRandomList(500);
	TestResults b = constantPartitionSizeTest('B', listOfSize500, 500, batchSize);
	TestResults h = constantListSizeTest('H', listOfSize500, 125, batchSize);
	TestResults i = constantListSizeTest('I', listOfSize500, 84, batchSize);
	TestResults j = constantListSizeTest('J', listOfSize500, 63, batchSize);
	TestResults k = constantListSizeTest('K', listOfSize500, 50, batchSize);
    TestResults l = constantListSizeTest('L', listOfSize500, 42, batchSize);
    TestResults m = constantListSizeTest('M', listOfSize500, 36, batchSize);
	TestResults n = constantListSizeTest('N', listOfSize500, 5, batchSize);
	TestResults o = constantListSizeTest('O', listOfSize500, 10, batchSize);
	TestResults p = constantListSizeTest('P', listOfSize500, 1, batchSize);
	free(listOfSize500);
*/

    printf("Running in %s mode *******************\n", RUNNINGMODE);
/*
	printf("Test A: List Size = 250 | Partition Size = 250 | 1 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", a.mean, a.stdDev, a.min, a.max);
	printf("Test B: List Size = 500 | Partition Size = 250 | 2 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", b.mean, b.stdDev, b.min, b.max);
	printf("Test C: List Size = 1000 | Partition Size = 250 | 4 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", c.mean, c.stdDev, c.min, c.max);
	printf("Test D: List Size = 2000 | Partition Size = 250 | 8 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", d.mean, d.stdDev, d.min, d.max);
	printf("Test E: List Size = 4000 | Partition Size = 250 | 16 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", e.mean, e.stdDev, e.min, e.max);
    printf("Test F: List Size = 6000 | Partition Size = 250 | 24 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", f.mean, f.stdDev, f.min, f.max);
	printf("Test G: List Size = 8000 | Partition Size = 250 | 32 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n\n", g.mean, g.stdDev, g.min, g.max);

	printf("Test H: List Size = 500 | Partition Size = 125 | 4 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", h.mean, h.stdDev, h.min, h.max);
	printf("Test I: List Size = 500 | Partition Size = 84 | 6 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", i.mean, i.stdDev, i.min, i.max);
	printf("Test J: List Size = 500 | Partition Size = 63 | 8 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", j.mean, j.stdDev, j.min, j.max);
	printf("Test K: List Size = 500 | Partition Size = 50 | 10 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", k.mean, k.stdDev, k.min, k.max);
    printf("Test L: List Size = 500 | Partition Size = 42 | 12 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", l.mean, l.stdDev, l.min, l.max);
    printf("Test M: List Size = 500 | Partition Size = 36 | 14 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", m.mean, m.stdDev, m.min, m.max);
	printf("Test N: List Size = 500 | Partition Size = 5 | 100 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", n.mean, n.stdDev, n.min, n.max);
	printf("Test O: List Size = 500 | Partition Size = 10 | 50 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", o.mean, o.stdDev, o.min, o.max);
	printf("Test P: List Size = 500 | Partition Size = 1 | 500 %s\n", RUNNINGMODE);
        printf("\tResults -> Mean: %ld | StdDev: %ld | Min: %ld | Max: %ld\n\n", p.mean, p.stdDev, p.min, p.max);	
*/
	return 0;
}
