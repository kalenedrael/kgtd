#include "globals.h"
#include "attr.h"

struct state_t {
	int score;
	int gil;
	int kills;
	int leaks;
	int total_noobs;
	int power_used;
	attr_t type_selected;
	path_t *path;
};

void state_reset(state_t *state);

