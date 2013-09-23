#ifndef _NOOB_H_
#define _NOOB_H_

#include "globals.h"
#include "tower_types.h"
#include "util.h"
#include "variable_queue.h"

#define ARMOR_COMPOSITE  1
#define ARMOR_REFLECTIVE 2

struct noob_t {
	Q_NEW_LINK(noob_t) list;
	path_t *path;
	pos_t pos;
	float speed;
	int hp;
	int max_hp;
	int shield;
	int max_shield;
	int stun_time;
	unsigned int refcnt;
	unsigned char armor_type;
	unsigned char future_stun;
	unsigned char is_dead;
};

void noob_init();
noob_t *noob_spawn(float speed, int hp, int shield, unsigned char armor_type,
                   state_t *state);
void noob_ref_destroy(noob_t *noob);

void noob_draw_all();
void noob_update_all(float dt, int idt, state_t *state);

noob_t *noob_find_target(pos_t *pos, ttype_t type);

#endif
