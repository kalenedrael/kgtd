/* @file tower.c
 * @brief tower management functions
 */

#include <float.h>
#include "grid_objs.h"
#include "noob.h"
#include "bullet.h"
#include "damage.h"

static Q_HEAD(tower_t) tower_list;

static void update_cw(tower_t *tower, int dt);
static void update_pulse(tower_t *tower, int dt);
static void update_proj(tower_t *tower, int dt);

/* @brief initialize towers and tower display lists */
void tower_init()
{
	Q_INIT_HEAD(&tower_list);

	glNewList(DISPLAY_LIST_TOWER, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(-TOWER_SIZE/2, -TOWER_SIZE/2);
	glVertex2f(-TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2, -TOWER_SIZE/2);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-TOWER_SIZE/2, -TOWER_SIZE/2);
	glVertex2f(-TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2, -TOWER_SIZE/2);
	glVertex2f(-TOWER_SIZE/2, -TOWER_SIZE/2);
	glEnd();
	glEndList();
}

/* @brief common tower init function */
static tower_t *tower_new_common(int x, int y, unsigned int power, attr_t attr,
                                 void (*update)(tower_t*, int))
{
	if(grid[y][x].type != GRID_TYPE_NONE)
		return NULL;

	tower_t *tower = &(grid[y][x].t);

	tower->type = GRID_TYPE_TOWER;
	tower->pos.x = x * GRID_SIZE + GRID_SIZE/2;
	tower->pos.y = y * GRID_SIZE + GRID_SIZE/2;
	tower->power = power;
	tower->energy = 0;
	tower->energymax = TOWER_DEFAULT_MAX_ENERGY;
	tower->attr = attr;
	tower->update = update;
	tower->target = NULL;

	Q_INSERT_HEAD(&tower_list, tower, list);

	return tower;
}

/* @brief creates a continuous-beam tower (red laser, blue plasma) */
tower_t *tower_new_cw(int x, int y, unsigned int power, attr_t attr)
{
	return tower_new_common(x, y, power, attr, update_cw);
}

/* @brief creates a pulse-firing tower (pink laser, stun) */
tower_t *tower_new_pulse(int x, int y, unsigned int power, attr_t attr)
{
	return tower_new_common(x, y, power, attr, update_pulse);
}

/* @brief creates a projectile tower */
tower_t *tower_new_proj(int x, int y, unsigned int power, attr_t attr)
{
	return tower_new_common(x, y, power, attr, update_proj);
}

/* @brief destroys a tower */
void tower_destroy(int x, int y)
{
	tower_t *tower = &(grid[x][y].t);
	Q_REMOVE(&tower_list, tower, list);
	grid[x][y].type = GRID_TYPE_NONE;
}

/* @brief common update function */
static void update_common(tower_t *tower, int dt)
{
	float cur_range = FLT_MAX;

	if(tower->energy < tower->energymax)
		tower->energy += tower->power * dt;

	if(tower->target != NULL)
		cur_range = distance2(&tower->pos, &tower->target->pos);

	if(cur_range > BULLET_MAX_RANGE ||
	   damage_not_worthwhile(tower->target, tower->attr))
		tower->target = noob_find_target(&tower->pos, tower->attr);
}

/* @brief updates a continuous-beam tower
 * 
 * A continuous-beam tower fires until the target dies or goes out of range.
 * Switching targets is not instantaneous since the tower must accumulate enough
 * energy to fire before firing another beam.
 */
static void update_cw(tower_t *tower, int dt)
{
	float cur_range = FLT_MAX;

	if(tower->energy < tower->energymax) {
		tower->energy += tower->power * dt;
		return;
	}

	/* no target; find one and fire at it */
	if(tower->target == NULL) {
		tower->target = noob_find_target(&tower->pos, tower->attr);
		if(tower->target == NULL)
			return;
		bullet_new_cw(&tower->pos, tower->power * 3000 /* XXX damage fix */,
		              tower->attr, tower->target);
		return;
	}
	cur_range = distance2(&tower->pos, &tower->target->pos);

	/* need to find new target */
	if(cur_range > BULLET_MAX_RANGE ||
	   damage_not_worthwhile(tower->target, tower->attr)) {
		/* set target to NULL to search for new target */
		tower->target = NULL;
		tower->energy -= tower->energymax;
	}
}

/* @brief update for pulse-type towers
 * 
 * The only difference between this and update_proj is the type of bullet
 * created
 */
static void update_pulse(tower_t *tower, int dt)
{
	update_common(tower, dt);
	if(tower->target != NULL && tower->energy >= tower->energymax) {
		tower->energy -= tower->energymax;
		bullet_new_pulse(&tower->pos, tower->energymax,
		                 tower->attr, tower->target);
	}
}

/* @brief update for projectile towers */
static void update_proj(tower_t *tower, int dt)
{
	update_common(tower, dt);
	if(tower->target != NULL && tower->energy >= tower->energymax) {
		tower->energy -= tower->energymax;
		bullet_new_proj(&tower->pos, tower->energymax,
		                tower->attr, tower->target);
	}
}

/* @brief common tower draw function
 *
 * This probably needs to be changed in the future to draw different icons for
 * different tower types.
 */
static void draw_each(tower_t *tower)
{
	float *clr = attr_colors[tower->attr];
	float scale = (float)tower->energy / (float)tower->energymax;

	if(scale >= 1.0)
		scale = 1.0;
	glColor4f(clr[0], clr[1], clr[2], scale);

	glPushMatrix();
	glTranslatef(tower->pos.x, tower->pos.y, 0);
	glCallList(DISPLAY_LIST_TOWER);
	glPopMatrix();
}

void tower_update_all(int dt)
{
	tower_t *cur;

	Q_FOREACH(cur, &tower_list, list)
		cur->update(cur, dt);
}

void tower_draw_all()
{
	tower_t *cur;

	Q_FOREACH(cur, &tower_list, list)
		draw_each(cur);
}
