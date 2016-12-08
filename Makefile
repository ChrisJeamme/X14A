CC=gcc
CFLAGS=-Wall
EXEC=initial
all: $(EXEC)

$(EXEC): initial.o
	$(CC) -o $@ $^

initial.o: initial.c
	$(CC) -o $@ -c $< $(CFLAGS)

EXEC2=journalistes
all: $(EXEC2)

$(EXEC2): journalistes.o
	$(CC) -o $@ $^

journalistes.o: journalistes.c
	$(CC) -o $@ -c $< $(CFLAGS)

EXEC3=archiviste
all: $(EXEC3)

$(EXEC3): archiviste.o
	$(CC) -o $@ $^

archiviste.o: archiviste.c
	$(CC) -o $@ -c $< $(CFLAGS)


clean:
	rm -rf *.o initial journalistes archiviste