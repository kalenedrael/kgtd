#include "level.h"
#include "noob.h"
#include "state.h"
#include "grid_objs.h"

void level_init(state_t *state)
{
	state->level = levels;
	state->wave = levels->waves;
	path_load(state, &state->level->map);
}

void level_update(state_t *state)
{
	wave_t *wave;
	level_t *level = state->level;

	if(level == NULL)
		return;

	wave = state->wave;
	if(state->kills + state->leaks >= wave->noobs) {
		if(wave->next == NULL) {
			state->level = level->next;
			if(state->level != NULL) {
				state->wave = state->level->waves;
				tower_init();
				grid_init();
				state->towers = 0;
				state->power_used = 0;
				path_load(state, &level->next->map);
			}
		}
		else {
			state->wave = wave->next;
		}

		state->kills = 0;
		state->leaks = 0;
		state->level_noobs = 0;
	}
}

void level_spawn(state_t *state)
{
	level_t *level = state->level;
	wave_t *wave = state->wave;

	if(level == NULL || state->level_noobs < wave->noobs) {
		noob_spawn(wave->speed, wave->hp, wave->shield,
		           wave->armor_type, wave->shield_type, state);
		state->level_noobs++;
	}
}
