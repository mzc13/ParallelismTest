all:
	echo "Must specify make proc or make thread"
proc: multitest_proc.o
	gcc searchtest.c multitest_proc.o -lm -o searchtest -std=c99 -Wall
thread: multitest_thread.o
	gcc searchtest.c multitest_thread.o -lm -lpthread -o searchtest -std=c99 -Wall
multitest_proc.o: 
	gcc -c multitest_proc.c -std=c99 -Wall
multitest_thread.o:
	gcc -c multitest_thread.c -std=c99 -Wall
clean:
	rm -f searchtest multitest_proc.o multitest_thread.o