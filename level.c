#include "level.h"
#include "noob.h"
#include "state.h"
#include "grid_objs.h"

static int path[] = {3, 7, 2, -7, 2, 5, 4, 6, 12};

static wave_t wave_2 = {
	.next = NULL,
	.noobs = 50,
	.hp = NOOB_DEFAULT_HP * 5,
	.shield = 0,
	.armor_type = 0,
	.shield_type = SHIELD_HARD
};

static wave_t wave_1 = {
	.next = &wave_2,
	.noobs = 10,
	.hp = NOOB_DEFAULT_HP,
	.shield = 0,
	.armor_type = 0,
	.shield_type = SHIELD_HARD
};

static level_t level_1 = {
	.next = NULL,
	.map = { path, sizeof(path)/sizeof(*path), 2, 3 },
	.n_waves = 1,
	.waves = &wave_1
};

void level_init(state_t *state)
{
	state->level = &level_1;
	state->wave = level_1.waves;
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
				path_load(state, &level->map);
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
		noob_spawn(wave->hp, wave->shield, wave->armor_type,
		           wave->shield_type, state);
		state->level_noobs++;
	}
}
