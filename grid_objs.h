#ifndef _GRID_H_
#define _GRID_H_

#include "globals.h"
#include "attr.h"
#include "util.h"
#include "variable_queue.h"

typedef enum grid_type {
	GRID_TYPE_NONE,
	GRID_TYPE_TOWER,
	GRID_TYPE_PATH
} grid_type;

struct tower_t {
	grid_type type;
	pos_t pos;
	unsigned int power;
	unsigned int energy;
	unsigned int energymax;
	float range;
	attr_t attr;
	noob_t *target;
	void (*update)(tower_t*, int);
	Q_NEW_LINK(tower_t) list;
};

struct path_t {
	grid_type type;
	pos_t pos;
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
tower_t *tower_new_cw(int x, int y, unsigned int power, attr_t attr);
tower_t *tower_new_pulse(int x, int y, unsigned int power, attr_t attr);
tower_t *tower_new_proj(int x, int y, unsigned int power, attr_t attr);
void tower_destroy(int x, int y);
void tower_update_all(int dt);
void tower_draw_all();
void tower_init();

/* path stuff */
path_t *path_new(int x, int y);
void path_destroy(int x, int y);
void path_load(state_t *state, map_t *map);
void path_draw_all(state_t *state);

#endif
