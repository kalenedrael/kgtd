#include "state.h"

void state_reset(state_t *state)
{
	memset(state, 0, sizeof(*state));
	state->selected = ATTR_BASIC;
	state->max_power = 3700;
}
