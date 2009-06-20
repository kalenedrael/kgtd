#include "state.h"

void state_reset(state_t *state)
{
	state->type_selected = ATTR_MASS_KINETIC;
	state->gil = 0;
	state->score = 0;
	state->kills = 0;
	state->leaks = 0;
	state->total_noobs = 0;
	state->power_used = 0;
	state->path = NULL;
}
