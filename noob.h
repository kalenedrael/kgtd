#ifndef _NOOB_H_
#define _NOOB_H_

#include "globals.h"
#include "attr.h"
#include "util.h"
#include "variable_queue.h"

enum {
	NOOB_ALIVE = 0,
	NOOB_KILLED = 1,
	NOOB_LEAKED = 2
};

#define ARMOR_COMPOSITE  1
#define ARMOR_REFLECTIVE 2

struct noob_t {
	pos_t pos;
	float speed;
	int hp;
	int max_hp;
	int shield;
	int max_shield;
	int stun_time;
	path_t *path;
	Q_NEW_LINK(noob_t) list;
	unsigned int refcnt;
	unsigned char armor_type;
	unsigned char future_stun;
	unsigned char is_dead;
};

typedef union noob_obj {
	noob_t n;
	union noob_obj *next;
} noob_obj;

void noob_init();
noob_t *noob_spawn(float speed, int hp, int shield, unsigned char armor_type,
                   state_t *state);
void noob_ref_destroy(noob_t *noob, state_t *state);

void noob_draw_all();
void noob_update_all(float dt, int idt, state_t *state);

noob_t *noob_find_target(pos_t *pos, attr_t attr);

#endif
