
t2: task2.o utils.o word_filter.o
	g++ task2.o utils.o word_filter.o -o t2

utils.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g ../utilities/function_timer.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g ../task_1/word_filter.cpp

task2.o: task2.cpp
	g++ -c -Wall -Werror -g task2.cpp

clean:
	rm *.o t2

# target: dependencies
#	action