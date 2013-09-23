#include "noob.h"
#include "state.h"
#include "grid_objs.h"
#include "damage.h"

#define NOOBS_NUM_MAX 1024
static noob_t noobs_pool[NOOBS_NUM_MAX];
static Q_HEAD(noob_t) noob_free;
Q_HEAD(noob_t) noob_list;

void noob_init()
{
	int i;

	Q_INIT_HEAD(&noob_free);
	Q_INIT_HEAD(&noob_list);

	for(i = 0; i < NOOBS_NUM_MAX; i++)
		Q_INSERT_HEAD(&noob_free, &noobs_pool[i], list);
}

noob_t *noob_spawn(float speed, int hp, int shield, unsigned char armor_type,
                   state_t *state)
{
	noob_t *noob = Q_GET_HEAD(&noob_free);
	if(noob == NULL) {
		printf("Too many noobs...\n");
		abort();
	}
	Q_REMOVE(&noob_free, noob, list);

	noob->pos = state->path->pos;
	noob->speed = speed;
	noob->hp = hp;
	noob->max_hp = hp;
	noob->shield = shield;
	noob->max_shield = shield;
	noob->armor_type = armor_type;
	noob->future_stun = 0;
	noob->stun_time = 0;
	noob->is_dead = 0;
	noob->refcnt = 1;
	noob->path = state->path;

	state->total_noobs++;
	Q_INSERT_HEAD(&noob_list, noob, list);

	return noob;
}

void noob_ref_destroy(noob_t *noob)
{
	if(--noob->refcnt)
		return;

	Q_REMOVE(&noob_list, noob, list);
	Q_INSERT_HEAD(&noob_free, noob, list);
}

static void draw_each(noob_t *noob)
{
	float scale, shield_scale;

	if(noob->is_dead)
		return;
	if(noob->stun_time > 0)
		glColor3f(0.0, 0.0, 1.0);
	else
		glColor4f(1.0, 1.0, 0.0, 0.4);

	scale = 0.2 + (float)noob->hp / noob->max_hp;
	glPushMatrix();
	glTranslatef(noob->pos.x, noob->pos.y, 0);
	glPushMatrix();
	glScalef(scale, scale, 1.0);
	glCallList(DISPLAY_LIST_NOOB);
	glPopMatrix();

	if(noob->shield > 0) {
		shield_scale = 0.2 + (float)noob->shield / noob->max_shield;
		glScalef(shield_scale, shield_scale, 1.0);
		glColor4f(0.1, 0.5, 1.0, 1.0);
		glCallList(DISPLAY_LIST_NOOB);
	}
	glPopMatrix();
}

static inline float move_dir(float d, float *loc, float dest)
{
	float l;

	if((l = fabs(dest - *loc)) < d) {
		*loc = dest;
		return l;
	}
	if(dest > *loc)
		*loc += d;
	else
		*loc -= d;
	return d;
}

static void update_each(noob_t *noob, float dt, int idt, state_t *state)
{
	float dl;
	path_t *path;

	if(noob->hp <= 0 && !(noob->is_dead)) {
		state->gil += 100 + 75 * (state->max_power - state->power_used) /
		                         (state->max_power);
		state->kills++;
		noob->is_dead = 1;
		noob_ref_destroy(noob);
		return;
	}

	if(noob->stun_time > 0) {
		noob->stun_time -= idt;
		if(noob->stun_time <= 0)
			noob->future_stun = 0;
		return;
	}

	for(dl = dt * noob->speed; dl > 0; /* no */) {
		path = noob->path;
		if(path == NULL) {
			state->leaks++;
			noob->is_dead = 1;
			noob_ref_destroy(noob);
			return;
		}
		if(noob->pos.x == path->pos.x) {
			dl -= move_dir(dl, &noob->pos.y, path->pos.y);
			if(noob->pos.y == path->pos.y)
				noob->path = path->next;
		}
		else {
			dl -= move_dir(dl, &noob->pos.x, path->pos.x);
			if(noob->pos.x == path->pos.x)
				noob->path = path->next;
		}
	}
}

void noob_update_all(float dt, int idt, state_t *state)
{
	noob_t *cur;

	Q_FOREACH(cur, &noob_list, list)
		update_each(cur, dt, idt, state);
}

void noob_draw_all()
{
	noob_t *cur;

	Q_FOREACH(cur, &noob_list, list)
		draw_each(cur);
}

noob_t *noob_find_target(pos_t *pos, ttype_t type)
{
	float mag, min_range = tt_data[type].tower.range;
	noob_t *noob = NULL, *cur;

	min_range = min_range * min_range;
	Q_FOREACH(cur, &noob_list, list) {
		if(damage_not_worthwhile(cur, type))
			continue;

		mag = distance2(pos, &cur->pos);
		if(mag < min_range) {
			min_range = mag;
			noob = cur;
		}
	}
	if(noob)
		noob->refcnt++;
	return noob;
}
