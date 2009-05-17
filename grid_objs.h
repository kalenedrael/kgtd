#ifndef _GRID_H_
#define _GRID_H_

#include "globals.h"
#include "attr.h"
#include "variable_queue.h"

typedef enum __attribute__((__packed__)) grid_type {
	GRID_TYPE_NONE,
	GRID_TYPE_TOWER,
	GRID_TYPE_PATH
} grid_type;

struct tower_t {
	grid_type type;
	float x;
	float y;
	float power;
	float energy;
	float energymax;
	float range;
	attr_t attr;
	noob_t *target;
	Q_NEW_LINK(tower_t) list;
};

struct path_t {
	grid_type type;
	float x;
	float y;
	struct path_t *next;
};

typedef union grid_t {
	grid_type type;
	tower_t t;
	path_t p;
} grid_t;

extern grid_t grid[GRID_Y][GRID_X];
void grid_init();

/* tower stuff */
tower_t *tower_new(int x, int y, float power, attr_t attr);
void tower_destroy(int x, int y);
void tower_update_all(float dt);
void tower_draw_all();
void tower_init();
void tower_traverse(void (*traverse_fn)(tower_t *, void *), void *data);

/* path stuff */
path_t *path_new(int x, int y);
void path_destroy(int x, int y);
void path_load(state_t *state, map_t *map);
void path_draw_all(state_t *state);

#endif

