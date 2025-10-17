.PHONY: all clean

CC ?= cc
CFLAGS   += -Wall -g
CPPFLAGS += -I .

all: rbtree.o

rbtree.o: rbtree.c rbtree.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f *.o