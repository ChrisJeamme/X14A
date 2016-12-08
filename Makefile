CC=gcc
CFLAGS=-Wall
EXEC=initial

all: $(EXEC)

$(EXEC): initial.o archiviste.o journalistes.o
	$(CC) -o $@ $^

journalistes.o: journalistes.c
	$(CC) -o $@ -c $< $(CFLAGS)

archiviste.o: archiviste.c
	$(CC) -o $@ -c $< $(CFLAGS)

initial.o: initial.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o initial