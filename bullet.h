#ifndef _BULLET_H_
#define _BULLET_H_

#include "globals.h"
#include "attr.h"

struct bullet_t {
	float x;
	float y;
	float xv;
	float yv;
	float damage;
	float age;
	noob_t *dest;
	attr_t attr;
};

typedef union bullet_obj {
	bullet_t b;
	struct {
		int is_valid;
		union bullet_obj *next;
	};
} bullet_obj;

void bullet_init();
bullet_t *bullet_new(float x, float y, float xv, float yv, float damage, attr_t attr, noob_t *dest);
void bullet_destroy(bullet_t *tower);

void bullet_update_all(float dt);
void bullet_draw_all();

void bullet_traverse(void (*traverse_fn)(bullet_t *, void *), void *data);

#endif
