#include "state.h"

void state_reset(state_t *state)
{
	state->selected = ATTR_BASIC;
	state->gil = 0;
	state->score = 0;
	state->kills = 0;
	state->leaks = 0;
	state->level_noobs = 0;
	state->total_noobs = 0;
	state->power_used = 0;
	state->towers = 0;
	state->path = NULL;
	state->level = NULL;
	state->wave = NULL;
}
