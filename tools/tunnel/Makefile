CC = g++
CLIBS = -lpthread
CFLAGS = -O4

OFILES = main.o LibComm.o

HFILES = LibComm.h

all:	tunnel

tunnel:	$(OFILES) $(HFILES) Makefile
	$(CC) -o $@ $(CFLAGS) $(OFILES) $(CLIBS)
	rm -f *~
	rm -f *#
	rm -f core*

.cpp.o: $< $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f *~
