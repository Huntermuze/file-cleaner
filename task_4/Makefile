
task4: task4.o utils.o word_filter.o function_timer.o
	g++ task4.o utils.o word_filter.o function_timer.o -pthread -o task4

utils.o: ../utilities/utils.h ../utilities/utils.cpp
	g++ -c -Wall -Werror -g -O3 ../utilities/utils.cpp

function_timer.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g -O3 ../utilities/function_timer.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g -O3 ../task_1/word_filter.cpp

task4.o: task4.cpp
	g++ -c -Wall -g -O3 task4.cpp

clean:
	rm *.o task4

# target: dependencies
#	action