#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "globals.h"

struct map_t {
	int *path;
	int length;
	int x_start;
	int y_start;
};

struct wave_t {
	wave_t *next;
	int noobs;
	int hp;
	int shield;
	int delay;
	float speed;
	unsigned char armor_type;
};

struct level_t {
	level_t *next;
	map_t map;
	wave_t *waves;
};

void level_init(state_t *state);
void level_update(int dt, state_t *state);

extern level_t *levels;

#endif
