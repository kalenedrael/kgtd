/* @file bullet.c
 * @brief bullet management functions
 *
 * 'bullet' here refers to anything that a tower fires, including beams.
 */

#include <float.h>
#include "bullet.h"
#include "damage.h"
#include "state.h"

#define BULLET_NUM_MAX 2048
static bullet_t bullet_pool[BULLET_NUM_MAX];
static Q_HEAD(bullet_t) bullet_list = Q_STATIC_INITIALIZER;
static Q_HEAD(bullet_t) bullet_free = Q_STATIC_INITIALIZER;
extern Q_HEAD(noob_t) noob_list;

/* @brief initialize bullet list */
void bullet_init()
{
	int i;

	for(i = 0; i < BULLET_NUM_MAX; i++)
		Q_INSERT_HEAD(&bullet_free, &bullet_pool[i], list);
}

/* @brief creates a new bullet of the given type */
bullet_t *bullet_new(pos_t *pos, unsigned int damage, ttype_t type, noob_t *dest)
{
	bullet_t *bullet = Q_GET_HEAD(&bullet_free);
	if(bullet == NULL) {
		printf("Too many bullets...\n");
		exit(0);
	}
	Q_REMOVE(&bullet_free, bullet, list);

	bullet->update = tt_data[type].bullet.update;
	bullet->draw = tt_data[type].bullet.draw;
	bullet->pos = *pos;
	bullet->damage = damage;
	bullet->age = 0;
	bullet->max_age = tt_data[type].bullet.max_age;
	bullet->range = tt_data[type].tower.range * tt_data[type].tower.range;
	bullet->type = type;
	bullet->dest = dest;

	dest->refcnt++;
	Q_INSERT_HEAD(&bullet_list, bullet, list);
	return bullet;
}

void bullet_destroy(bullet_t *bullet)
{
	noob_ref_destroy(bullet->dest);
	Q_REMOVE(&bullet_list, bullet, list);
	Q_INSERT_HEAD(&bullet_free, bullet, list);
}

static void set_color(bullet_t *bullet)
{
	float *clr = tt_data[bullet->type].color;
	float trans = 1.0 - (float)bullet->age / (float)bullet->max_age;
	glColor4f(clr[0], clr[1], clr[2], trans);
}

static void draw_hit_glow(pos_t *pos)
{
	glPushMatrix();
	glTranslatef(pos->x, pos->y, 0.0);
	glScalef(20.0, 20.0, 1.0);
	glCallList(DISPLAY_LIST_GLOW);
	glPopMatrix();
}

/* @brief draw a projectile-type bullet */
void bullet_draw_proj(bullet_t *bullet)
{
	set_color(bullet);
	if(bullet->age == 0) {
		glBegin(GL_QUADS);
		glVertex2f(bullet->pos.x, bullet->pos.y);
		glVertex2f(bullet->pos.x, bullet->pos.y + BULLET_SIZE);
		glVertex2f(bullet->pos.x + BULLET_SIZE, bullet->pos.y + BULLET_SIZE);
		glVertex2f(bullet->pos.x + BULLET_SIZE, bullet->pos.y);
		glEnd();
	}
	else {
		draw_hit_glow(&bullet->pos);
	}
}

/* @brief draws a beam-type bullet */
void bullet_draw_beam(bullet_t *bullet)
{
	set_color(bullet);
	glBegin(GL_LINES);
	glVertex2f(bullet->pos.x, bullet->pos.y);
	glVertex2f(bullet->dest->pos.x, bullet->dest->pos.y);
	glEnd();
	draw_hit_glow(&bullet->dest->pos);
}

void bullet_draw_area(bullet_t *bullet)
{
	float range = tt_data[TT_AREA].tower.range * 1.2;

	set_color(bullet);
	glPushMatrix();
	glTranslatef(bullet->pos.x, bullet->pos.y, 0.0);
	glScalef(range, range, 1.0);
	glCallList(DISPLAY_LIST_GLOW);
	glPopMatrix();
}

static void overpenetrate(bullet_t *bullet, float angle)
{
	noob_t *cur, *opt = NULL;
	float d2, da, score = FLT_MAX;

	Q_FOREACH(cur, &noob_list, list) {
		if(cur == bullet->dest)
			continue;
		d2 = distance2(&bullet->pos, &cur->pos);
		da = fabs(angle - atan2(cur->pos.y - bullet->pos.y,
		                        cur->pos.x - bullet->pos.x));
		if(d2 < 10000.0 && da < 0.1 && d2 * da < score && !cur->is_dead) {
			opt = cur;
			score = d2 * da;
		}
	}

	if(opt != NULL && bullet->damage > 10000)
		bullet_new(&bullet->pos, bullet->damage * 3 / 4, bullet->type, opt);

	return;
}

/* @brief updates a projectile-type bullet */
void bullet_upd_proj(bullet_t *bullet, float dt, int idt)
{
	float new_x, new_y, dy, dx, dm;
	float xv, yv;
	float dest_x = bullet->dest->pos.x, dest_y = bullet->dest->pos.y;

	if(bullet->age > 0) {
		/* bullet has completely faded; kill */
		if(bullet->age > bullet->max_age)
			bullet_destroy(bullet);
		else
			bullet->age += idt;
		return;
	}

	/* calculate new position of bullet - moves by BULLET_SPEED * dt in the
	 * direction of the target
	 */
	dx = dest_x - bullet->pos.x;
	dy = dest_y - bullet->pos.y;
	dm = sqrt(dx * dx + dy * dy);
	xv = BULLET_SPEED * (dx / dm);
	yv = BULLET_SPEED * (dy / dm);
	new_x = bullet->pos.x + xv * dt;
	new_y = bullet->pos.y + yv * dt;

	/* see if we hit the target */
	if(((bullet->pos.x >= dest_x && new_x <= dest_x) ||
	    (bullet->pos.x <= dest_x && new_x >= dest_x)) &&
	   ((bullet->pos.y >= dest_y && new_y <= dest_y) ||
	    (bullet->pos.y <= dest_y && new_y >= dest_y)))
	{
		damage_calc(bullet, idt);
		bullet->age = 1;
		bullet->pos.x = dest_x;
		bullet->pos.y = dest_y;
		if(bullet->type == TT_APFSDS || bullet->type == TT_DU)
			overpenetrate(bullet, atan2f(yv, xv));
	}
	else {
		bullet->pos.x = new_x;
		bullet->pos.y = new_y;
	}
}

/* @brief updates a continuous-beam bullet
 *
 * in a cw bullet, age represents the time after the beam stops firing in ms;
 * this is used to fade the bullet
 */
void bullet_upd_cw(bullet_t *bullet, float dt, int idt)
{
	/* bullet has completely faded; kill */
	if(bullet->age > bullet->max_age) {
		bullet_destroy(bullet);
		return;
	}
	/* target dead or out of range; fade the beam by incrementing age */
	if(bullet->dest->is_dead || distance2(&bullet->pos, &bullet->dest->pos) > bullet->range)
		bullet->age += idt;
	else
		/* do damage every frame */
		damage_calc(bullet, idt);
}

/* @brief updates a pulse bullet */
void bullet_upd_pulse(bullet_t *bullet, float dt, int idt)
{
	int life = bullet->max_age - bullet->age;

	bullet->age += idt;
	/* bullet lasts a fixed time */
	if(life < 0)
		bullet_destroy(bullet);
	else
		damage_calc(bullet, idt > life ? life : idt);
}

void bullet_update_all(float dt, int idt)
{
	bullet_t *cur, *nxt;
	Q_FOREACH_NXT(cur, nxt, &bullet_list, list)
		cur->update(cur, dt, idt);
}

void bullet_draw_all()
{
	bullet_t *cur;
	Q_FOREACH(cur, &bullet_list, list)
		cur->draw(cur);
}
