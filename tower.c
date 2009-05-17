#include "grid_objs.h"
#include "noob.h"
#include "bullet.h"
#include "damage.h"

static Q_HEAD(tower_t) tower_list;

tower_t *tower_new(int x, int y, float power, attr_t attr)
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
}

void tower_destroy(int x, int y)
{
	tower_t *tower = &(grid[x][y].t);
	Q_REMOVE(&tower_list, tower, list);
	grid[x][y].type = GRID_TYPE_NONE;
}

static void update_each(tower_t *tower, void *dtp)
{
	float dt = *(float*)dtp;

	if(tower->energy < tower->energymax)
		tower->energy += tower->power * dt;

	if(damage_not_worthwhile(tower->target, tower->attr)) {
		tower->target = find_target(tower->x + GRID_SIZE/2,
                                            tower->y + GRID_SIZE/2, tower->attr);
		if(tower->target == NULL)
			return;
	}

	/* fire! */
	if(tower->energy > tower->energymax) {
		tower->energy -= tower->energymax;

		bullet_new(tower->x + GRID_SIZE/2, tower->y + GRID_SIZE/2, 0.0, 0.0,
		           tower->energymax, tower->attr, tower->target);
	}	
}

static void draw_each(tower_t *tower, void* bs)
{
	float r, g, b, scale, tx, txo, ty, tyo;

	scale = tower->energy / tower->energymax;
	if(scale >= 1.0) {
		glColor3fv(attr_colors[tower->attr]);
	} else {
		r = attr_colors[tower->attr][0] * scale;
		g = attr_colors[tower->attr][1] * scale;
		b = attr_colors[tower->attr][2] * scale;
		glColor3f(r, g, b);
	}

	tx = tower->x;
	ty = tower->y;
	txo = tx + GRID_SIZE;
	tyo = ty + GRID_SIZE;

	glBegin(GL_QUADS);
	glVertex2f(tx,  ty);
	glVertex2f(tx,  tyo);
	glVertex2f(txo, tyo);
	glVertex2f(txo, ty);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(tx,  ty);
	glVertex2f(tx,  tyo);
	glVertex2f(txo, tyo);
	glVertex2f(txo, ty);
	glVertex2f(tx,  ty);
	glEnd();
}

void tower_update_all(float dt)
{
	tower_traverse(&update_each, &dt);
}

void tower_draw_all()
{
	tower_traverse(&draw_each, NULL);
}

void tower_traverse(void (*traverse_fn)(tower_t *, void *), void *data)
{
	tower_t *cur;

	Q_FOREACH(cur, &tower_list, list) {
		traverse_fn(cur, data);
	}
}

