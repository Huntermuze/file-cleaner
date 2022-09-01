
t2: task2.o function_timer.o word_filter.o
	g++ task2.o function_timer.o word_filter.o -o t2

task2.o: task2.cpp ../task_1/word_filter.h ../task_1/word_filter.cpp ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g task2.cpp ../utilities/function_timer.cpp ../task_1/word_filter.cpp

clean:
	rm *.o t2

# target: dependencies
#	action