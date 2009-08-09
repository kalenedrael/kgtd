CC=gcc
CFLAGS=-g -O2 -mfpmath=sse -msse -mmmx -Wall -ffast-math
LIBS=-lSDLmain -lSDL -lGL

ALL_OBJS=tower.o bullet.o kgtd.o noob.o damage.o grid.o state.o controls.o \
         attr.o path.o graphics.o level.o level_data.o

all: kgtd

clean:
	rm -f kgtd *.o *~

depend:
	$(CC) -MM *.c > .depend
	
kgtd: $(ALL_OBJS)
	$(CC) -o kgtd $(LIBS) $(ALL_OBJS)

-include .depend
