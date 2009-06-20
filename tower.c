#include <float.h>
#include "grid_objs.h"
#include "noob.h"
#include "bullet.h"
#include "damage.h"

static Q_HEAD(tower_t) tower_list;

tower_t *tower_new(int x, int y, int power, attr_t attr)
{
	if(grid[y][x].type != GRID_TYPE_NONE) {
		return NULL;
	}

	tower_t *tower = &(grid[y][x].t);

	tower->type = GRID_TYPE_TOWER;
	tower->x = x * GRID_SIZE;
	tower->y = y * GRID_SIZE;
	tower->power = power;
	tower->energy = 0;
	tower->energymax = TOWER_DEFAULT_MAX_ENERGY;
	tower->attr = attr;
	tower->target = NULL;

	Q_INSERT_HEAD(&tower_list, tower, list);

	return tower;
}

void tower_init()
{
	Q_INIT_HEAD(&tower_list);

	glNewList(DISPLAY_LIST_TOWER, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(0.0,        0.0);
	glVertex2f(0.0,        TOWER_SIZE);
	glVertex2f(TOWER_SIZE, TOWER_SIZE);
	glVertex2f(TOWER_SIZE, 0.0);
	glEnd();
	glEndList();
}

void tower_destroy(int x, int y)
{
	tower_t *tower = &(grid[x][y].t);
	Q_REMOVE(&tower_list, tower, list);
	grid[x][y].type = GRID_TYPE_NONE;
}

static void update_each(tower_t *tower, float dt, int idt)
{
	float cur_range = FLT_MAX, dx, dy;

	if(tower->energy < tower->energymax)
		tower->energy += tower->power * idt;

	if(tower->target != NULL) {
		dx = tower->x - tower->target->x;
		dy = tower->y - tower->target->y;
		cur_range = dx * dx + dy * dy;
	}
	if(cur_range > BULLET_MAX_RANGE ||
	   damage_not_worthwhile(tower->target, tower->attr)) {
		tower->target = find_target(tower->x + GRID_SIZE/2,
                                            tower->y + GRID_SIZE/2,
		                            tower->attr);
		if(tower->target == NULL)
			return;
	}

	/* fire! */
	if(tower->energy >= tower->energymax) {
		tower->energy -= tower->energymax;

		bullet_new(tower->x + GRID_SIZE/2, tower->y + GRID_SIZE/2, 0.0, 0.0,
		           tower->energymax, tower->attr, tower->target);
	}
}

static void draw_each(tower_t *tower)
{
	float r, g, b, scale;

	scale = (float)tower->energy / (float)tower->energymax;
	if(scale >= 1.0) {
		glColor3fv(attr_colors[tower->attr]);
	} else {
		r = attr_colors[tower->attr][0] * scale;
		g = attr_colors[tower->attr][1] * scale;
		b = attr_colors[tower->attr][2] * scale;
		glColor3f(r, g, b);
	}

	glPushMatrix();
	glTranslatef(tower->x + (GRID_SIZE - TOWER_SIZE) / 2,
	             tower->y + (GRID_SIZE - TOWER_SIZE) / 2, 0);
	glCallList(DISPLAY_LIST_TOWER);
	glPopMatrix();
}

void tower_update_all(float dt, int idt)
{
	tower_t *cur;

	Q_FOREACH(cur, &tower_list, list)
		update_each(cur, dt, idt);
}

void tower_draw_all()
{
	tower_t *cur;

	Q_FOREACH(cur, &tower_list, list)
		draw_each(cur);
}

void tower_traverse(void (*traverse_fn)(tower_t *, void *), void *data)
{
	tower_t *cur;

	Q_FOREACH(cur, &tower_list, list)
		traverse_fn(cur, data);
}

