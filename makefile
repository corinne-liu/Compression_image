CC = gcc
CFLAGS = -Wall -std=c17 -pedantic
INCLUDE_DIR = include

EXEC = codec

all: $(EXEC)

$(EXEC): obj/main.o
	$(CC) $(CFLAGS) -o $(EXEC) obj/main.o -Wl,-rpath=lib/ -Llib/ -lqtc -lm

obj/main.o: src/main.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR) -Ilib/

clean:
	rm -f $(EXEC) 
	rm -r obj
