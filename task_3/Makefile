
task3: task3.o utils.o word_filter.o function_timer.o
	g++ task3.o utils.o word_filter.o function_timer.o -o task3

utils.o: ../utilities/utils.h ../utilities/utils.cpp
	g++ -c -Wall -Werror -g -O3 ../utilities/utils.cpp

function_timer.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g -O3 ../utilities/function_timer.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g -O3 ../task_1/word_filter.cpp

task3.o: task3.cpp
	g++ -c -Wall -g -O3 task3.cpp

clean:
	rm *.o task3

# target: dependencies
#	action