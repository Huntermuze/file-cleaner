task5: task5.o word_filter.o utils.o
	g++ task5.o word_filter.o utils.o -pthread -o task5

utils.o: ../utilities/utils.cpp ../utilities/utils.h
	g++ -c -Wall -Werror -g -O3 ../utilities/utils.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g -O3 ../task_1/word_filter.cpp

task5.o: task5.cpp
	g++ -c -Wall -g -O3 task5.cpp

clean:
	rm *.o task5

# target: dependencies
#	action