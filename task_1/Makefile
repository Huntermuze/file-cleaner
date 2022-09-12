
task1: task1.o function_timer.o word_filter.o utils.o
	g++ task1.o function_timer.o word_filter.o utils.o -pthread -o task1

utils.o: ../utilities/utils.cpp ../utilities/utils.h
	g++ -c -Wall -Werror -g -O3 ../utilities/utils.cpp

function_timer.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g -O3 ../utilities/function_timer.cpp

word_filter.o: word_filter.h word_filter.cpp
	g++ -c -Wall -Werror -g -O3 word_filter.cpp

task1.o: task1.cpp
	g++ -c -Wall -Werror -g -O3 task1.cpp

clean:
	rm *.o task1

# target: dependencies
#	action