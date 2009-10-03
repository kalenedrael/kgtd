#include "level.h"
#include "noob.h"
#include "state.h"
#include "grid_objs.h"

static int until_spawn;

void level_init(state_t *state)
{
	state->level = levels;
	state->wave = levels->waves;
	path_load(state, &state->level->map);
	until_spawn = 0;
	state->until_next = WAVE_DELAY * 1000;
}


void level_update(int dt, state_t *state)
{
	int next = state->until_next;
	wave_t *wave = state->wave;
	level_t *level = state->level;

	next = next > dt ? next - dt : 0;

	/* do not advance wave until we are done spawning noobs */
	if(next == 0 && level != NULL && state->level_noobs >= wave->noobs) {
		if(wave->next != NULL) {
			state->wave = wave = wave->next;
			next = WAVE_DELAY * 1000;
			state->level_noobs = 0;
		}
		/* only advance level if we have killed/leaked all noobs */
		else if(state->kills + state->leaks >= state->total_noobs) {
			state->level = level->next;
			if(state->level != NULL) {
				state->wave = wave = state->level->waves;
				tower_init();
				grid_init();
				state->towers = 0;
				state->power_used = 0;
				path_load(state, &level->next->map);
			}
			next = WAVE_DELAY * 1000;
			state->level_noobs = 0;
		}
	}
	state->until_next = next;

	until_spawn = until_spawn > dt ? until_spawn - dt : 0;
	if(state->level_noobs == wave->noobs)
		return;
	if(until_spawn == 0) {
		noob_spawn(wave->speed, wave->hp, wave->shield,
		           wave->armor_type, wave->shield_type, state);
		if(level != NULL)
			state->level_noobs++;
		until_spawn += wave->delay;
	}
}
