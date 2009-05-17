#include "noob.h"
#include "state.h"
#include "grid_objs.h"
#include "damage.h"

static noob_obj noobs[NOOBS_NUM_MAX];
static noob_obj *noob_first_free;

void noob_init()
{
	int i;

	for(i = 0; i < NOOBS_NUM_MAX - 1; i++) {
		noobs[i].is_valid = 0;
		noobs[i].next = &(noobs[i+1]);
	}

	noobs[i].is_valid = 0;
	noobs[i].next = NULL;
	noob_first_free = noobs;
}

noob_t *noob_new(float x, float y, state_t *state)
{
	noob_obj *n_obj = noob_first_free;
	if(n_obj == NULL) {
		printf("Fuck, too many noobs.\n");
		abort();
	}
	noob_first_free = n_obj->next;

	noob_t *noob = &(n_obj->n);
	noob->x = x;
	noob->y = y;
	noob->hp = NOOB_DEFAULT_HP;
	noob->future_stun = 0;
	noob->stun_time = 0;
	noob->is_dead = 0;
	noob->refcnt = 0;
	state->total_noobs++;

	return noob;
}

void noob_destroy(noob_t *noob, state_t *state)
{
	noob_obj *n_obj = (noob_obj*)noob;

	if(noob->is_dead == 1)
		state->kills++;
	else if(noob->is_dead == 2)
		state->leaks++;
	n_obj->is_valid = 0;
	n_obj->next = noob_first_free;
	noob_first_free = n_obj;
}

static void draw_each(noob_t *noob, void *bs)
{
	/* XXX */
	if(noob->is_dead) {
		return;
	}
	if(noob->stun_time > 0) {
		glColor3d(0.0, 0.0, 1.0);
	}
	else {
		glColor4d(1.0, 1.0, 0.0, 0.4);
	}
	glBegin(GL_QUADS);
	glVertex2i(noob->x - NOOB_SIZE/2.0, noob->y - NOOB_SIZE/2.0);
	glVertex2i(noob->x - NOOB_SIZE/2.0, noob->y + NOOB_SIZE/2.0);
	glVertex2i(noob->x + NOOB_SIZE/2.0, noob->y + NOOB_SIZE/2.0);
	glVertex2i(noob->x + NOOB_SIZE/2.0, noob->y - NOOB_SIZE/2.0);
	glEnd();
}

static void update_each(noob_t *noob, float dt, state_t *state)
{
	float dx, dy;
	int x, y;
	path_t *dest;

	if(noob->hp <= 0 && !(noob->is_dead)) {
		noob->is_dead = 1;
	}
	if(noob->is_dead) {
		if(noob->refcnt == 0)
			noob_destroy(noob, state);
		return;
	}

	if(noob->stun_time > 0) {
		noob->stun_time--;
		if(noob->stun_time == 0) {
			noob->future_stun = 0;
		}
	}
	else {
		x = noob->x * (1.0/GRID_SIZE);
		y = noob->y * (1.0/GRID_SIZE);

		dest = grid[y][x].p.next;

		if(dest == NULL || noob->y > YRES || noob->y < 0.0 ||
		   noob->x > XRES || noob->x < 0.0)
		{
			noob->is_dead = 2;
			return;
		}

		dx = dest->x - noob->x;
		dy = dest->y - noob->y;
		noob->x += dx * dt * NOOB_SPEED_MULT;
		noob->y += dy * dt * NOOB_SPEED_MULT;

	}
}

void noob_update_all(float dt, state_t *state)
{
	int i;
	for(i = 0; i < NOOBS_NUM_MAX; i++) {
		if(noobs[i].is_valid) {
			update_each(&(noobs[i].n), dt, state);
		}		
	}
}

void noob_draw_all()
{
	noob_traverse(draw_each, NULL);
}

void noob_traverse(void (*traverse_fn)(noob_t *, void *), void *data)
{
	int i;
	for(i = 0; i < NOOBS_NUM_MAX; i++) {
		if(noobs[i].is_valid) {
			traverse_fn(&(noobs[i].n), data);
		}		
	}
}

noob_t *find_target(float x, float y, attr_t attr)
{
	/* XXX max_range wtf */

	int i;
	float d_closest = BULLET_MAX_RANGE;
	noob_t *n_closest = NULL;

	for(i = 0; i < NOOBS_NUM_MAX; i++) {
		/* XXX separate */
		if(noobs[i].is_valid) {
			float d_x, d_y, mag;

			if(damage_not_worthwhile(&(noobs[i].n), attr))
				continue;

			d_x = noobs[i].n.x - x;
			d_y = noobs[i].n.y - y;
			mag = d_x * d_x + d_y * d_y;
			if(mag < d_closest) {
				d_closest = mag;
				n_closest = &(noobs[i].n);
			}
		}
	}

	return n_closest;
}

