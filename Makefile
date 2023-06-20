CC = g++ -std=c++17
CCFLAGS = -Wall -O3

target = client server
obj = client.o server.o

includeDir = ./
tmpDir = ../tmp

all: $(target)

$(obj): %.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean: 
	rm ${target} *.o

