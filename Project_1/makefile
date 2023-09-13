CC = g++
CFLAGS = -Wall -std=c++11

TARGETS = doit justBlank

all: $(TARGETS)

myexec: myexec.cpp
	$(CC) $(CFLAGS) -o myexec myexec.cpp

Fork: Fork.cpp
	$(CC) $(CFLAGS) -o Fork Fork.cpp

doit: doit.cpp
	$(CC) $(CFLAGS) -o doit doit.cpp

justBlank: justBlank.cpp
	$(CC) $(CFLAGS) -o justBlank justBlank.cpp

clean:
	rm -f $(TARGETS)