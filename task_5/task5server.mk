task5server: task5server.o word_filter.o
	g++ task5server.o word_filter.o -pthread -o task5server

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g -O3 ../task_1/word_filter.cpp

task5server.o: task5server.cpp
	g++ -c -Wall -g -O3 task5server.cpp

clean:
	rm *.o task5server

# target: dependencies
#	action