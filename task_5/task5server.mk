task5server: task5server.o word_filter.o function_timer.o
	g++ task5server.o word_filter.o function_timer.o -o task5server

utils.o: ../utilities/utils.cpp ../utilities/utils.h
	g++ -c -Wall -Werror -g ../utilities/utils.cpp

function_timer.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g ../utilities/function_timer.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g ../task_1/word_filter.cpp

task5server.o: task5server.cpp
	g++ -c -Wall -g task5server.cpp

clean:
	rm *.o task5server

# target: dependencies
#	action