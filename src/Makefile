CC = g++
CFLAGS = -Wall
BIN = ../bin/
OBJS = qrsfilter.o dataio.o qrsdet.o fqrsdet.o

fqrsdet: $(OBJS)
	$(CC) $(OBJS) -o $(BIN)fqrsdet

qrsfilter.o: qrsfilter.c qrsfilter.h
	$(CC) $(CFLAGS) -c qrsfilter.c -o qrsfilter.o

dataio.o: dataio.c dataio.h qrsdet.h
	$(CC) $(CFLAGS) -c dataio.c -o dataio.o

qrsdet.o: qrsdet.c qrsdet.h qrsfilter.h
	$(CC) $(CFLAGS) -c qrsdet.c -o qrsdet.o

fqrsdet.o: fqrsdet.cpp qrsdet.h qrsfilter.h dataio.h
	$(CC) $(CFLAGS) -c fqrsdet.cpp -o fqrsdet.o

clean:
	rm -f $(BIN)fqrsdet $(OBJS)
