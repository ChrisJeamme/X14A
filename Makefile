CC=gcc
CFLAGS=-Wall
EXEC=initial

all: $(EXEC)

$(EXEC): initial.o
	$(CC) -o $@ $^

initial.o: initial.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o initial