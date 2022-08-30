
t1: Task2.o
	g++ Task2.o -o t2

Task1.o: Task2.cpp Task2.h
	g++ -c -Wall -Werror -g Task2.cpp

clean:
	rm *.o t2

# target: dependencies
#	action