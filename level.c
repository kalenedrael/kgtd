#include "level.h"
#include "noob.h"
#include "state.h"
#include "grid_objs.h"

static wave_t wave12 = {
	.next = NULL,
	.noobs = 50,
	.hp = NOOB_DEFAULT_HP * 2,
	.shield = 0,
	.armor_type = 0,
	.shield_type = SHIELD_HARD
};

static wave_t wave11 = {
	.next = &wave12,
	.noobs = 10,
	.hp = NOOB_DEFAULT_HP,
	.shield = 0,
	.armor_type = 0,
	.shield_type = SHIELD_HARD
};

static wave_t wave21 = {
	.next = NULL,
	.noobs = 50,
	.hp = NOOB_DEFAULT_HP * 5,
	.shield = 0,
	.armor_type = 0,
	.shield_type = SHIELD_HARD
};

static int path1[] = {3, 7, 2, -7, 2, 5, 4, 6, 12};
static int path2[] = {8, 5, 2, -10, 2, 5, 8};

static level_t level2 = {
	.next = NULL,
	.map = { path2, sizeof(path2)/sizeof(*path2), 3, 9 },
	.waves = &wave21
};

static level_t level1 = {
	.next = &level2,
	.map = { path1, sizeof(path1)/sizeof(*path1), 2, 3 },
	.waves = &wave11
};

void level_init(state_t *state)
{
	state->level = &level1;
	state->wave = level1.waves;
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
		noob_spawn(wave->hp, wave->shield, wave->armor_type,
		           wave->shield_type, state);
		state->level_noobs++;
	}
}
