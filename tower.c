/* @file tower.c
 * @brief tower management functions
 */

#include <float.h>
#include "grid_objs.h"
#include "noob.h"
#include "bullet.h"
#include "damage.h"

#define PWR_IN 0
#define PWR_OUT 2

static Q_HEAD(tower_t) tower_list;

/* @brief initialize towers... used to have more */
void tower_init()
{
	Q_INIT_HEAD(&tower_list);
}

/* @brief creates a tower
 * @return the created tower, or NULL if the desired position is occupied
 */
tower_t *tower_new(int x, int y, unsigned int power, ttype_t type)
{
	if(grid[y][x].g != GRID_TYPE_NONE)
		return NULL;

	tower_t *tower = &(grid[y][x].t);

	tower->g = GRID_TYPE_TOWER;
	tower->pos.x = x * GRID_SIZE + GRID_SIZE/2;
	tower->pos.y = y * GRID_SIZE + GRID_SIZE/2;
	tower->power = power;
	tower->energy = 0;
	tower->energymax = TOWER_DEFAULT_MAX_ENERGY;
	tower->type = type;
	tower->range = tt_data[type].tower.range * tt_data[type].tower.range;
	tower->update = tt_data[type].tower.upd;
	tower->target = NULL;

	Q_INSERT_HEAD(&tower_list, tower, list);

	return tower;
}

/* @brief destroys a tower */
void tower_destroy(int x, int y)
{
	tower_t *tower = &(grid[x][y].t);
	Q_REMOVE(&tower_list, tower, list);
	grid[x][y].g = GRID_TYPE_NONE;
}

/* @brief updates a continuous-beam tower
 * 
 * A continuous-beam tower fires until the target dies or goes out of range.
 * Switching targets is not instantaneous since the tower must accumulate enough
 * energy to fire before firing another beam.
 */
void tower_upd_cw(tower_t *tower, int dt)
{
	float cur_range = FLT_MAX;

	if(tower->power == 0)
		return;

	if(tower->energy < tower->energymax) {
		tower->energy += (tower->type == TT_PLASMA ? 150 : 50) * dt;
		return;
	}

	/* no target; find one and fire at it */
	if(tower->target == NULL) {
		tower->target = noob_find_target(&tower->pos, tower->type);
		if(tower->target == NULL)
			return;
		bullet_new(&tower->pos, tower->power * 1024 /* damage fix */,
		           tower->type, tower->target);
		return;
	}
	cur_range = distance2(&tower->pos, &tower->target->pos);

	/* need to find new target */
	if(cur_range > tower->range ||
	   damage_not_worthwhile(tower->target, tower->type)) {
		/* set target to NULL to search for new target */
		tower->target = NULL;
		tower->energy -= tower->energymax;
	}
}

/* @brief update for pulse and projectile towers
 *
 * A tower charges until it is ready to fire, then it fires at its current
 * target if it is in range or worth firing at; otherwise, it looks for the
 * closest target in range
 */
void tower_upd_normal(tower_t *tower, int dt)
{
	noob_t *target = tower->target;

	/* de-target */
	if(damage_not_worthwhile(target, tower->type) ||
	   (target != NULL && distance2(&tower->pos, &target->pos) > tower->range))
		target = NULL;

	if(tower->energy < tower->energymax) {
		tower->energy += tower->power * dt;
		goto out;
	}

	/* acquire target */
	if(target == NULL) {
		target = noob_find_target(&tower->pos, tower->type);
		if(target == NULL)
			goto out;
	}

	tower->energy -= tower->energymax;
	bullet_new(&tower->pos, tower->energymax, tower->type, target);
out:
	tower->target = target;
	return;
}

/* @brief common tower draw function */
static void draw_each(tower_t *tower)
{
	float *clr = tt_data[tower->type].color;
	float scale = (float)tower->energy / (float)tower->energymax;
	float power_dist = ((TOWER_SIZE - PWR_IN + PWR_OUT) / (float)TOWER_MAX_PWR *
	                   tower->power) + PWR_IN;

	if(scale >= 1.0)
		scale = 1.0;
	glColor4f(clr[0], clr[1], clr[2], scale);

	/* draw tower icon with  */
	glPushMatrix();
	glTranslatef(tower->pos.x, tower->pos.y, 0);
	glCallList(DISPLAY_LIST_TOWER);
	glColor3f(1.0, 1.0, 1.0);
	glCallList(DISPLAY_LIST_TOWER_BASE + tower->type);

	/* draw power indicator */
	glBegin(GL_QUADS);
	glVertex2f(-TOWER_SIZE/2 - PWR_OUT, -TOWER_SIZE/2 - PWR_OUT);
	glVertex2f(-TOWER_SIZE/2 + power_dist, -TOWER_SIZE/2 - PWR_OUT);
	glVertex2f(-TOWER_SIZE/2 + power_dist, -TOWER_SIZE/2 + PWR_IN);
	glVertex2f(-TOWER_SIZE/2 - PWR_OUT, -TOWER_SIZE/2 + PWR_IN);

	glVertex2f(-TOWER_SIZE/2 - PWR_OUT, -TOWER_SIZE/2 - PWR_OUT);
	glVertex2f(-TOWER_SIZE/2 - PWR_OUT, -TOWER_SIZE/2 + power_dist);
	glVertex2f(-TOWER_SIZE/2 + PWR_IN, -TOWER_SIZE/2 + power_dist);
	glVertex2f(-TOWER_SIZE/2 + PWR_IN, -TOWER_SIZE/2 - PWR_OUT);

	glVertex2f(TOWER_SIZE/2 + PWR_OUT, TOWER_SIZE/2 + PWR_OUT);
	glVertex2f(TOWER_SIZE/2 - power_dist, TOWER_SIZE/2 + PWR_OUT);
	glVertex2f(TOWER_SIZE/2 - power_dist, TOWER_SIZE/2 - PWR_IN);
	glVertex2f(TOWER_SIZE/2 + PWR_OUT, TOWER_SIZE/2 - PWR_IN);

	glVertex2f(TOWER_SIZE/2 + PWR_OUT, TOWER_SIZE/2 + PWR_OUT);
	glVertex2f(TOWER_SIZE/2 - power_dist, TOWER_SIZE/2 + PWR_OUT);
	glVertex2f(TOWER_SIZE/2 - power_dist, TOWER_SIZE/2 - PWR_IN);
	glVertex2f(TOWER_SIZE/2 + PWR_OUT, TOWER_SIZE/2 - PWR_IN);

	glVertex2f(TOWER_SIZE/2 + PWR_OUT, TOWER_SIZE/2 + PWR_OUT);
	glVertex2f(TOWER_SIZE/2 + PWR_OUT, TOWER_SIZE/2 - power_dist);
	glVertex2f(TOWER_SIZE/2 - PWR_IN, TOWER_SIZE/2 - power_dist);
	glVertex2f(TOWER_SIZE/2 - PWR_IN, TOWER_SIZE/2 + PWR_OUT);
	glEnd();
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
