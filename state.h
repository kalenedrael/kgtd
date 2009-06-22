#include "globals.h"
#include "attr.h"

struct state_t {
	int score;
	int gil;
	int kills;
	int leaks;
	int level_noobs;
	int total_noobs;
	int power_used;
	attr_t type_selected;
	path_t *path;
	level_t *level;
	wave_t *wave;
};

void state_reset(state_t *state);

