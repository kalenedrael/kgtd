CC=gcc
CFLAGS=-g -O2 -mfpmath=sse -msse -mmmx -Wall -ffast-math
LIBS=-lSDL -lGL -lSDL_ttf

ALL_OBJS=tower.o bullet.o kgtd.o noob.o damage.o grid.o state.o controls.o attr.o path.o

all: kgtd

clean:
	rm -f kgtd *.o *~

depend:
	$(CC) -MM *.c > .depend
	
kgtd: $(ALL_OBJS)
	$(CC) -o kgtd $(LIBS) $(ALL_OBJS)

-include .depend
