#ifndef _MULTITEST_H
#define _MULTITEST_H

#define dummySearch(W, X, Y, Z) parallelSearch((W), (X), (Y), (Z))

extern char* RUNNINGMODE;

int parallelSearch(int* list, int listSize, int partitionSize, int targetValue);

#endif