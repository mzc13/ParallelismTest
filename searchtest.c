#include<sys/time.h>
#include<limits.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include "multitest.h"

// Comment out this line for a random seed on every execution
#define time(NULL) 0

// Struct to hold results of each test
typedef struct _TestResults {
	long mean;
	long stdDev;
	long min;
	long max;
} TestResults;

// Generate a random list of integer
int* generateRandomList(int size) {
	// Initializing list
	int* list = malloc(sizeof(int) * size);
	for (int i = 0; i < size; i++)
		list[i] = i;

	// Initializing variables for randomizing list
	int randIndex0 = rand() % size;
	int randIndex1 = rand() % size;
	int temp = -1;
	// Swap random indices (size * 0.8) times
	for (int i = 0; i < (size * 0.8); i++) {
		temp = list[randIndex0];
		list[randIndex0] = list[randIndex1];
		list[randIndex1] = temp;

		randIndex0 = rand() % size;
		randIndex1 = rand() % size;
	}

	return list;
}

// Generate "new" list by changing location of value at index
int generateNewList(int* list, int size, int index) {
	int randIndex = rand() % size;
	int temp = list[index];
	list[index] = list[randIndex];
	list[randIndex] = temp;
	return 0;
}

// Returns average value of array
long getMean(long times[], int size) {
	long total = 0;
	int i = 0;
	for (i = 0; i < size; i++)
		total += times[i];
	return total / size;
}

// Returns standard deviation of array
long getStdDev(long times[], int size, long mean) {
	double numeratorSum = 0;
	for (int i = 0; i < size; i++)
		numeratorSum += pow(times[i] - mean, 2);
	return sqrt(numeratorSum / (size - 1));
}

// Returns time taken to find target. Also, prints the results of this iteration.
long testTime(char testIdentifier, int* list, int listSize, int partitionSize, int target, int iteration, int* index) {
	struct timeval start, end;
	gettimeofday(&start, NULL);

	// Index value holds index where target was found
	*index = dummySearch(list, listSize, partitionSize, target);
	gettimeofday(&end, NULL);
	int seconds = end.tv_sec - start.tv_sec;
	int microSeconds = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;

	printf("Test %c-%d | Found %d at : %d\n", testIdentifier, iteration, target, *index);
	return microSeconds;
}

// Runs test where partition size is constant
TestResults constantPartitionSizeTest(char testIndicator, int* list, int listSize, int numberOfRuns) {
	// Initializing variables
	int partitionSize = 250;
	int target = listSize / 2;
	int targetIndex = -1;
	long times[numberOfRuns];
	long min = __LONG_MAX__;
	long max = -1;

	// Run the test the number of times specified
	for (int i = 0; i < numberOfRuns; i++) {
		times[i] = testTime(testIndicator, list, listSize, partitionSize, target, i, &targetIndex);
		generateNewList(list, listSize, targetIndex);

		// If new min or max values are found, record them
		min = (times[i] < min) ? times[i] : min;
		max = (times[i] > max) ? times[i] : max;
	}

	// Preparing struct for return
	TestResults retVal;
	retVal.mean = getMean(times, numberOfRuns);
	retVal.stdDev = getStdDev(times, numberOfRuns, retVal.mean);
	retVal.min = min;
	retVal.max = max;
	return retVal;
}

// Runs test where list size is constant
TestResults constantListSizeTest(char testIndicator, int* list, int partitionSize, int numberOfRuns) {
	// Initializing variables
	int listSize = 500;
	int target = listSize / 2;
	int targetIndex = -1;
	long times[numberOfRuns];
	long min = __LONG_MAX__;
	long max = -1;

	// Run the test the number of times specified
	for (int i = 0; i < numberOfRuns; i++) {
		times[i] = testTime(testIndicator, list, listSize, partitionSize, target, i, &targetIndex);
		generateNewList(list, listSize, targetIndex);

		// If new min or max values are found, record them
		min = (times[i] < min) ? times[i] : min;
		max = (times[i] > max) ? times[i] : max;
	}

	// Preparing struct for return
	TestResults retVal;
	retVal.mean = getMean(times, numberOfRuns);
	retVal.stdDev = getStdDev(times, numberOfRuns, retVal.mean);
	retVal.min = min;
	retVal.max = max;
	return retVal;
}

int main() {
	srand(time(NULL));

	// Number of times each test runs
	int batchSize = 100;

	// Test A: List Size = 250 | Partition Size = 250 | 1 Process/Thread
	int* listOfSize250 = generateRandomList(250);
	TestResults a = constantPartitionSizeTest('A', listOfSize250, 250, batchSize);
	free(listOfSize250);
	listOfSize250 = NULL;

	// Test C: List Size = 1000 | Partition Size = 250 | 4 Processes/Threads
	int* listOfSize1000 = generateRandomList(1000);
	TestResults c = constantPartitionSizeTest('C', listOfSize1000, 1000, batchSize);
	free(listOfSize1000);
	listOfSize1000 = NULL;

	// Test D: List Size = 2000 | Partition Size = 250 | 8 Processes/Threads
	int* listOfSize2000 = generateRandomList(2000);
	TestResults d = constantPartitionSizeTest('D', listOfSize2000, 2000, batchSize);
	free(listOfSize2000);
	listOfSize2000 = NULL;

	// Test E: List Size = 4000 | Partition Size = 250 | 16 Processes/Threads
	int* listOfSize4000 = generateRandomList(4000);
	TestResults e = constantPartitionSizeTest('E', listOfSize4000, 4000, batchSize);
	free(listOfSize4000);
	listOfSize4000 = NULL;

	// Test F: List Size = 8000 | Partition Size = 250 | 32 Processes/Threads
	int* listOfSize8000 = generateRandomList(8000);
	TestResults f = constantPartitionSizeTest('F', listOfSize8000, 8000, batchSize);
	free(listOfSize8000);
	listOfSize8000 = NULL;

	// Test B: List Size = 500 | Partition Size = 250 | 2 Processes/Threads
	// Test B is run here because the list of size 500 is also used for the future tests
	// Placing it here prevents data skewing due to cpu caching
	int* listOfSize500 = generateRandomList(500);
	TestResults b = constantPartitionSizeTest('B', listOfSize500, 500, batchSize);

	// Test G: List Size = 500 | Partition Size = 100 | 5 Processes/Threads
	TestResults g = constantListSizeTest('G', listOfSize500, 100, batchSize);
	// Test H: List Size = 500 | Partition Size = 50 | 10 Processes/Threads
	TestResults h = constantListSizeTest('H', listOfSize500, 50, batchSize);
	// Test I: List Size = 500 | Partition Size = 25 | 20 Processes/Threads
	TestResults i = constantListSizeTest('I', listOfSize500, 25, batchSize);
	// Test J: List Size = 500 | Partition Size = 10 | 50 Processes/Threads
	TestResults j = constantListSizeTest('J', listOfSize500, 10, batchSize);
	// Test K: List Size = 500 | Partition Size = 5 | 100 Processes/Threads
	TestResults k = constantListSizeTest('K', listOfSize500, 5, batchSize);

	free(listOfSize500);

	// An external variable holds which mode the program is running in
	printf("Running in %s mode *******************\n", RUNNINGMODE);
	printf("Each test is run %d times.\n", batchSize);
	printf("Test A: List Size = 250 | Partition Size = 250 | 1 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", a.mean, a.stdDev, a.min, a.max);
	printf("Test B: List Size = 500 | Partition Size = 250 | 2 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", b.mean, b.stdDev, b.min, b.max);
	printf("Test C: List Size = 1000 | Partition Size = 250 | 4 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", c.mean, c.stdDev, c.min, c.max);
	printf("Test D: List Size = 2000 | Partition Size = 250 | 8 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", d.mean, d.stdDev, d.min, d.max);
	printf("Test E: List Size = 4000 | Partition Size = 250 | 16 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", e.mean, e.stdDev, e.min, e.max);
	printf("Test F: List Size = 8000 | Partition Size = 250 | 32 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n\n", f.mean, f.stdDev, f.min, f.max);

	printf("Test G: List Size = 500 | Partition Size = 100 | 5 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", g.mean, g.stdDev, g.min, g.max);
	printf("Test H: List Size = 500 | Partition Size = 50 | 10 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", h.mean, h.stdDev, h.min, h.max);
	printf("Test I: List Size = 500 | Partition Size = 25 | 20 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", i.mean, i.stdDev, i.min, i.max);
	printf("Test J: List Size = 500 | Partition Size = 10 | 50 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", j.mean, j.stdDev, j.min, j.max);
	printf("Test K: List Size = 500 | Partition Size = 5 | 100 %s\n", RUNNINGMODE);
	printf("\tResults -> Mean: %ld µs | StdDev: %ld μs | Min: %ld μs | Max: %ld μs\n\n", k.mean, k.stdDev, k.min, k.max);

	return 0;
}