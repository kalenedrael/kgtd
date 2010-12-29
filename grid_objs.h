#ifndef _GRID_H_
#define _GRID_H_

#include "globals.h"
#include "tower_types.h"
#include "util.h"
#include "variable_queue.h"

typedef enum grid_type {
	GRID_TYPE_NONE,
	GRID_TYPE_TOWER,
	GRID_TYPE_PATH
} grid_type;

struct tower_t {
	grid_type g;
	pos_t pos;
	unsigned int power;
	unsigned int energy;
	unsigned int energymax;
	float range;
	ttype_t type;
	noob_t *target;
	void (*update)(tower_t*, int);
	Q_NEW_LINK(tower_t) list;
};

struct path_t {
	grid_type g;
	pos_t pos;
	struct path_t *next;
};

typedef union grid_t {
	grid_type g;
	tower_t t;
	path_t p;
} grid_t;

extern grid_t grid[GRID_Y][GRID_X];
void grid_init();

/* tower stuff */
tower_t *tower_new(int x, int y, unsigned int power, ttype_t type);
void tower_destroy(int x, int y);
void tower_update_all(int dt);
void tower_draw_all();
void tower_init();

/* path stuff */
void path_load(state_t *state, map_t *map);
void path_draw_all(state_t *state);

#endif
