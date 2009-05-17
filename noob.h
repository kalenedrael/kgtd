#ifndef _NOOB_H_
#define _NOOB_H_

#include "globals.h"
#include "attr.h"
#include "variable_queue.h"

enum {
	NOOB_ALIVE = 0,
	NOOB_KILLED = 1,
	NOOB_LEAKED = 2
};

enum {
	SHIELD_HARD = 0,
	SHIELD_SOFT = 1,
	SHIELD_ADAPTIVE = 2
};

#define ARMOR_COMPOSITE  1
#define ARMOR_REACTIVE   2
#define ARMOR_REFLECTIVE 4
#define ARMOR_REGEN      8

struct noob_t {
	float x;
	float y;
	float hp;
	float shield;
	Q_NEW_LINK(noob_t) list;
	unsigned int refcnt;
	unsigned char future_stun;
	unsigned char stun_time;
	unsigned char is_dead;
	unsigned char armor_type;
	unsigned char shield_type;
};

typedef union noob_obj {
	noob_t n;
	struct {
		int is_valid;
		union noob_obj *next;
	};
} noob_obj;

void noob_init();
noob_t *noob_new(float x, float y, state_t *state);
void noob_destroy(noob_t *noob, state_t *state);

void noob_draw_all();
void noob_update_all(float dt, state_t *state);

void noob_traverse(void (*traverse_fn)(noob_t*, void*), void *data);

noob_t *find_target(float x, float y, attr_t attr);

#endif

