
t1: task1.o utils.o word_filter.o
	g++ task1.o utils.o word_filter.o -o t1

utils.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g ../utilities/function_timer.cpp

word_filter.o: word_filter.h word_filter.cpp
	g++ -c -Wall -Werror -g word_filter.cpp

task1.o: task1.cpp
	g++ -c -Wall -Werror -g task1.cpp

clean:
	rm *.o t1

# target: dependencies
#	action