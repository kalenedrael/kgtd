#ifndef _STATE_H_
#define _STATE_H_

#include "globals.h"
#include "tower_types.h"

struct state_t {
	int score;
	int gil;
	int kills;
	int leaks;
	int level_noobs;
	int total_noobs;
	int power_used;
	int max_power;
	int towers;
	int until_next;
	ttype_t selected;
	path_t *path;
	level_t *level;
	wave_t *wave;
};

void state_reset(state_t *state);

#endif /* _STATE_H_ */
