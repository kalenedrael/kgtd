#include "bullet.h"
#include "damage.h"
#include "state.h"

static bullet_obj bullets[BULLET_NUM_MAX];
static bullet_obj *bullet_first_free;

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

bullet_t *bullet_new(float x, float y, float xv, float yv, float damage, attr_t attr, noob_t *dest)
{
	bullet_obj *b_obj = bullet_first_free;
	if(b_obj == NULL) {
		printf("Fuck, too many bullets.\n");
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
	dest->refcnt++;

	return bullet;
}

void bullet_destroy(bullet_t *bullet)
{
	bullet_obj *b_obj = (bullet_obj*)bullet;

	bullet->dest->refcnt--;
	b_obj->is_valid = 0;
	b_obj->next = bullet_first_free;
	bullet_first_free = b_obj;
}

static void draw_each(bullet_t *bullet, void *throwaway)
{
	glColor3fv(attr_colors[bullet->attr]);

	if(!((bullet_obj*)bullet)->is_valid)
		return;
	if(!((noob_obj*)bullet->dest)->is_valid)
		return;

	switch(bullet->attr) {
	case ATTR_MASS_KINETIC:
	case ATTR_MASS_KINETIC_APCR:
	case ATTR_MASS_KINETIC_APFSDS:
	case ATTR_MASS_KINETIC_DU:
	case ATTR_MASS_EXPLOSIVE_HE:
	case ATTR_MASS_EXPLOSIVE_HEAT:
	case ATTR_MASS_EXPLOSIVE_HESH:
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
		glBegin(GL_LINES);
		glVertex2i(bullet->x, bullet->y);
		glVertex2i(bullet->dest->x, bullet->dest->y);
		glEnd();
		break;
	}
}

static void update_each(bullet_t *bullet, void *dtp)
{
	float new_x, new_y, dy, dx, dm;
	float new_xv, new_yv, speed;
	float dest_x = bullet->dest->x, dest_y = bullet->dest->y;
	float dt = *(float*)dtp;

	if(bullet->age > BULLET_MAX_AGE) {
		bullet_destroy(bullet);
		return;
	}

	if(!((noob_obj*)bullet->dest)->is_valid) {
		bullet_destroy(bullet);
		return;
	}

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
			damage_calc(bullet->dest, bullet->damage, dt, bullet->attr);
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
		if(bullet->age > BULLET_PULSE_MAX_AGE) {
			bullet_destroy(bullet);
			return;
		}
	case ATTR_ENERGY_PARTICLE_PLASMA:
	case ATTR_ENERGY_LASER_CW:
		damage_calc(bullet->dest, bullet->damage, dt, bullet->attr);
	}

	bullet->age += dt;
}

void bullet_update_all(float dt)
{
	bullet_traverse(&update_each, &dt);
}

void bullet_draw_all()
{
	bullet_traverse(&draw_each, NULL);
}

void bullet_traverse(void (*traverse_fn)(bullet_t *, void *), void *data)
{
	int i;
	for(i = 0; i < BULLET_NUM_MAX; i++) {
		if(bullets[i].is_valid) {
			traverse_fn(&(bullets[i].b), data);
		}		
	}
}
