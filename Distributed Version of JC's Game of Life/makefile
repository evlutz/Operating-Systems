# Compiler and Flags
CC = g++
CFLAGS = -std=c++11 -pthread


# Executable
EXEC = game_of_life

# Targets
all: $(EXEC)

$(EXEC): life.cpp life.h AbstractLife.h
	$(CC) $(CFLAGS) life.cpp -o $(EXEC)

clean:
	rm -f $(EXEC)
