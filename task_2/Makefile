
task2: task2.o function_timer.o word_filter.o utils.o
	g++ task2.o function_timer.o word_filter.o utils.o -o task2

function_timer.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g -O3 ../utilities/function_timer.cpp

utils.o: ../utilities/utils.cpp ../utilities/utils.h
	g++ -c -Wall -Werror -g -O3 ../utilities/utils.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g -O3 ../task_1/word_filter.cpp

task2.o: task2.cpp
	g++ -c -Wall -g -O3 task2.cpp

clean:
	rm *.o task2

# target: dependencies
#	action