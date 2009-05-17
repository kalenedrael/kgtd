#include "globals.h"
#include "attr.h"

struct state_t {
	int score;
	int gil;
	int kills;
	int leaks;
	int total_noobs;
	attr_t type_selected;
	path_t *path;
};

void state_reset(state_t *state);

