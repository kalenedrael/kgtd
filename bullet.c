#include "bullet.h"
#include "damage.h"
#include "state.h"

static bullet_obj bullets[BULLET_NUM_MAX];
static bullet_obj *bullet_first_free;
static Q_HEAD(bullet_t) bullet_list = Q_STATIC_INITIALIZER;

void bullet_init()
{
	int i;

	for(i = 0; i < BULLET_NUM_MAX - 1; i++) {
		bullets[i].is_valid = 0;
		bullets[i].next = &(bullets[i+1]);
	}

	bullets[i].is_valid = 0;
	bullets[i].next = NULL;
	bullet_first_free = bullets;
}

bullet_t *bullet_new(float x, float y, float xv, float yv, int damage, attr_t attr, noob_t *dest)
{
	bullet_obj *b_obj = bullet_first_free;
	if(b_obj == NULL) {
		printf("Too many bullets...\n");
		exit(0);
	}
	bullet_first_free = b_obj->next;

	bullet_t *bullet = &(b_obj->b);
	bullet->attr = attr;
	bullet->dest = dest;
	bullet->x = x;
	bullet->y = y;
	bullet->xv = xv;
	bullet->yv = yv;
	bullet->damage = damage;
	bullet->age = 0.0;
	if(attr == ATTR_ENERGY_LASER_PULSE ||
	   attr == ATTR_ENERGY_PARTICLE_LIGHTNING)
		bullet->max_age = BULLET_PULSE_MAX_AGE;
	else
		bullet->max_age = BULLET_MAX_AGE;
	dest->refcnt++;

	Q_INSERT_HEAD(&bullet_list, bullet, list);

	return bullet;
}

void bullet_destroy(bullet_t *bullet)
{
	bullet_obj *b_obj = (bullet_obj*)bullet;

	bullet->dest->refcnt--;
	Q_REMOVE(&bullet_list, bullet, list);
	b_obj->is_valid = 0;
	b_obj->next = bullet_first_free;
	bullet_first_free = b_obj;
}

static void draw_each(bullet_t *bullet)
{
	float *clr = attr_colors[bullet->attr];
	float trans;

	if(!((bullet_obj*)bullet)->is_valid)
		return;

	switch(bullet->attr) {
	case ATTR_MASS_KINETIC:
	case ATTR_MASS_KINETIC_APCR:
	case ATTR_MASS_KINETIC_APFSDS:
	case ATTR_MASS_KINETIC_DU:
	case ATTR_MASS_EXPLOSIVE_HE:
	case ATTR_MASS_EXPLOSIVE_HEAT:
	case ATTR_MASS_EXPLOSIVE_HESH:
		glColor3fv(clr);
		glBegin(GL_QUADS);
		glVertex2i(bullet->x, bullet->y);
		glVertex2i(bullet->x, bullet->y + BULLET_SIZE);
		glVertex2i(bullet->x + BULLET_SIZE, bullet->y + BULLET_SIZE);
		glVertex2i(bullet->x + BULLET_SIZE, bullet->y);
		glEnd();
		break;
	case ATTR_ENERGY_LASER_PULSE:
	case ATTR_ENERGY_PARTICLE_PLASMA:
	case ATTR_ENERGY_PARTICLE_LIGHTNING:
	case ATTR_ENERGY_LASER_CW:
		trans = 1.0 - (float)bullet->age / (float)bullet->max_age;
		glColor4f(clr[0], clr[1], clr[2], trans);
		glBegin(GL_LINES);
		glVertex2i(bullet->x, bullet->y);
		glVertex2i(bullet->dest->x, bullet->dest->y);
		glEnd();
		break;
	}
}

static void update_each(bullet_t *bullet, float dt, int idt)
{
	float new_x, new_y, dy, dx, dm;
	float new_xv, new_yv, speed;
	float dest_x = bullet->dest->x, dest_y = bullet->dest->y;

	if(bullet->age > bullet->max_age) {
		bullet_destroy(bullet);
		return;
	}
	bullet->age += idt;
	if(bullet->age > bullet->max_age)
		idt = bullet->age - bullet->max_age;

	switch(bullet->attr) {
	case ATTR_MASS_KINETIC:
	case ATTR_MASS_KINETIC_APCR:
	case ATTR_MASS_KINETIC_APFSDS:
	case ATTR_MASS_KINETIC_DU:
	case ATTR_MASS_EXPLOSIVE_HE:
	case ATTR_MASS_EXPLOSIVE_HEAT:
	case ATTR_MASS_EXPLOSIVE_HESH:
		speed = BULLET_DEFAULT_SPEED;

		dx = dest_x - bullet->x;
		dy = dest_y - bullet->y;
		dm = sqrt(dx * dx + dy * dy);
		new_xv = speed * (dx / dm);
		new_yv = speed * (dy / dm);

		new_x = bullet->x + new_xv * dt;
		new_y = bullet->y + new_yv * dt;

		/* delete if off screen */
		if(new_x < 0.0 || new_x >= XRES - BULLET_SIZE ||
		    new_y < 0.0 || new_y >= YRES - BULLET_SIZE)
		{
			bullet_destroy(bullet);
			return;
		}

		if(((bullet->x >= dest_x && new_x <= dest_x) ||
		    (bullet->x <= dest_x && new_x >= dest_x)) &&
		   ((bullet->y >= dest_y && new_y <= dest_y) ||
		    (bullet->y <= dest_y && new_y >= dest_y)))
		{
			damage_calc(bullet->dest, bullet->damage, idt, bullet->attr);
			bullet_destroy(bullet);
			return;
		}

		bullet->x = new_x;
		bullet->y = new_y;
		bullet->xv = new_xv;
		bullet->yv = new_yv;

		break;
	case ATTR_ENERGY_LASER_PULSE:
	case ATTR_ENERGY_PARTICLE_LIGHTNING:
	case ATTR_ENERGY_PARTICLE_PLASMA:
	case ATTR_ENERGY_LASER_CW:
		damage_calc(bullet->dest, bullet->damage, idt, bullet->attr);
	}

}

void bullet_update_all(float dt, int idt)
{
	bullet_t *cur;

	Q_FOREACH(cur, &bullet_list, list)
		update_each(cur, dt, idt);
}

void bullet_draw_all()
{
	bullet_t *cur;

	Q_FOREACH(cur, &bullet_list, list)
		draw_each(cur);
}

void bullet_traverse(void (*traverse_fn)(bullet_t *, void *), void *data)
{
	bullet_t *cur;

	Q_FOREACH(cur, &bullet_list, list)
		traverse_fn(cur, data);
}
