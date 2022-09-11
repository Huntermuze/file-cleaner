task5: task5.o word_filter.o function_timer.o utils.o
	g++ task5.o word_filter.o function_timer.o utils.o -o task5

utils.o: ../utilities/utils.cpp ../utilities/utils.h
	g++ -c -Wall -Werror -g ../utilities/utils.cpp

function_timer.o: ../utilities/function_timer.h ../utilities/function_timer.cpp
	g++ -c -Wall -Werror -g ../utilities/function_timer.cpp

word_filter.o: ../task_1/word_filter.h ../task_1/word_filter.cpp
	g++ -c -Wall -Werror -g ../task_1/word_filter.cpp

task5.o: task5.cpp
	g++ -c -Wall -g task5.cpp

clean:
	rm *.o task5

# target: dependencies
#	action