
t1: task1.o function_timer.o word_filter.o
	g++ task1.o function_timer.o word_filter.o -o t1

task1.o: task1.cpp word_filter.cpp word_filter.h ../utilities/function_timer.cpp ../utilities/function_timer.h
	g++ -c -Wall -Werror -g task1.cpp word_filter.cpp ../utilities/function_timer.cpp

clean:
	rm *.o t1

# target: dependencies
#	action