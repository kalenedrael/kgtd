CC=gcc
CFLAGS=-g -O2 -mfpmath=sse -msse -mmmx -Wall -ffast-math
LIBS=-lSDL -lGL -lSDL_ttf

ALL_OBJS=tower.o bullet.o kgtd.o noob.o damage.o grid.o state.o controls.o attr.o path.o

all: kgtd

clean:
	rm -f kgtd *.o *~

kgtd: $(ALL_OBJS)
	$(CC) -o kgtd $(LIBS) $(ALL_OBJS)

attr.o: attr.c attr.h globals.h
bullet.o: bullet.c bullet.h globals.h attr.h damage.h noob.h state.h
controls.o: controls.c controls.h globals.h state.h attr.h
damage.o: damage.c damage.h attr.h globals.h noob.h
grid.o: grid.c grid_objs.h globals.h attr.h
kgtd.o: kgtd.c globals.h grid_objs.h attr.h state.h controls.h noob.h \
  bullet.h
noob.o: noob.c noob.h globals.h attr.h state.h grid_objs.h damage.h
path.o: path.c grid_objs.h globals.h attr.h state.h
state.o: state.c state.h globals.h attr.h
tower.o: tower.c grid_objs.h globals.h attr.h noob.h bullet.h damage.h
