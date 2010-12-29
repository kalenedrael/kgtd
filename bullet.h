/* @file bullet.h
 * @brief bullet_t and bullet function declarations
 */

#ifndef _BULLET_H_
#define _BULLET_H_

#include "globals.h"
#include "tower_types.h"
#include "util.h"
#include "variable_queue.h"

struct bullet_t {
	void (*update)(bullet_t*, float, int);
	void (*draw)(bullet_t*);
	pos_t pos;
	unsigned int damage;
	unsigned int age;
	unsigned int max_age;
	ttype_t type;
	noob_t *dest;
	Q_NEW_LINK(bullet_t) list;
};

typedef union bullet_obj {
	bullet_t b;
	union bullet_obj *next;
} bullet_obj;

void bullet_init();
bullet_t *bullet_new(pos_t *pos, unsigned int damage, ttype_t type, noob_t *dest);
void bullet_destroy(bullet_t *tower);

void bullet_update_all(float dt, int idt);
void bullet_draw_all();
#endif
