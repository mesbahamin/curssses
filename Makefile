EXECUTABLE = curssses

build:
	gcc -Wall -Wextra -lcurses curssses.c -o $(EXECUTABLE)

run:
	./$(EXECUTABLE)

test: build run
