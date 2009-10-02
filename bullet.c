/* @file bullet.c
 * @brief bullet management functions
 *
 * 'bullet' here refers to anything that a tower fires, including beams.
 */

#include "bullet.h"
#include "damage.h"
#include "state.h"

static bullet_obj bullets[BULLET_NUM_MAX];
static bullet_obj *bullet_first_free;
static Q_HEAD(bullet_t) bullet_list;

static void update_pulse(bullet_t *bullet, float dt, int idt);
static void update_cw(bullet_t *bullet, float dt, int idt);
static void update_proj(bullet_t *bullet, float dt, int idt);
static void draw_beam(bullet_t *bullet);
static void draw_proj(bullet_t *bullet);

struct {
	void (*update)(bullet_t*, float, int);
	void (*draw)(bullet_t*);
	unsigned int max_age;
} bullet_attr[ATTR_NUM] = {
	{update_cw   , draw_beam, BULLET_MAX_AGE},
	{update_pulse, draw_beam, BULLET_PULSE_MAX_AGE},
	{update_cw   , draw_beam, BULLET_MAX_AGE},
	{update_pulse, draw_beam, BULLET_PULSE_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE},
	{update_proj , draw_proj, BULLET_MAX_AGE}
};

/* @brief initialize bullet list */
void bullet_init()
{
	int i;

	for(i = 0; i < BULLET_NUM_MAX - 1; i++)
		bullets[i].next = &(bullets[i+1]);

	bullets[i].next = NULL;
	bullet_first_free = bullets;
	Q_INIT_HEAD(&bullet_list);
}

/* @brief creates a new bullet of the given type */
bullet_t *bullet_new(pos_t *pos, unsigned int damage, attr_t attr, noob_t *dest)
{
	bullet_obj *b_obj = bullet_first_free;
	if(b_obj == NULL) {
		printf("Too many bullets...\n");
		exit(0);
	}
	bullet_first_free = b_obj->next;

	bullet_t *bullet = &b_obj->b;
	bullet->update = bullet_attr[attr].update;
	bullet->draw = bullet_attr[attr].draw;
	bullet->pos = *pos;
	bullet->damage = damage;
	bullet->age = 0;
	bullet->max_age = bullet_attr[attr].max_age;
	bullet->attr = attr;
	bullet->dest = dest;

	dest->refcnt++;
	Q_INSERT_HEAD(&bullet_list, bullet, list);
	return bullet;
}

void bullet_destroy(bullet_t *bullet)
{
	bullet_obj *b_obj = (bullet_obj*)bullet;

	bullet->dest->refcnt--;
	Q_REMOVE(&bullet_list, bullet, list);
	b_obj->next = bullet_first_free;
	bullet_first_free = b_obj;
}

/* @brief draw a projectile-type bullet */
static void draw_proj(bullet_t *bullet)
{
	glColor3fv(attr_colors[bullet->attr]);
	glBegin(GL_QUADS);
	glVertex2f(bullet->pos.x, bullet->pos.y);
	glVertex2f(bullet->pos.x, bullet->pos.y + BULLET_SIZE);
	glVertex2f(bullet->pos.x + BULLET_SIZE, bullet->pos.y + BULLET_SIZE);
	glVertex2f(bullet->pos.x + BULLET_SIZE, bullet->pos.y);
	glEnd();
}

/* @brief draws a beam-type bullet */
static void draw_beam(bullet_t *bullet)
{
	float *clr = attr_colors[bullet->attr];
	float trans = 1.0 - (float)bullet->age / (float)bullet->max_age;

	glColor4f(clr[0], clr[1], clr[2], trans);
	glBegin(GL_LINES);
	glVertex2f(bullet->pos.x, bullet->pos.y);
	glVertex2f(bullet->dest->pos.x, bullet->dest->pos.y);
	glEnd();
}

/* @brief updates a projectile-type bullet */
static void update_proj(bullet_t *bullet, float dt, int idt)
{
	float new_x, new_y, dy, dx, dm;
	float xv, yv;
	float dest_x = bullet->dest->pos.x, dest_y = bullet->dest->pos.y;

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
		damage_calc(bullet->dest, bullet->damage, idt, bullet->attr);
		bullet_destroy(bullet);
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
static void update_cw(bullet_t *bullet, float dt, int idt)
{
	/* bullet has completely faded; kill */
	if(bullet->age > bullet->max_age) {
		bullet_destroy(bullet);
		return;
	}
	/* target dead or out of range; fade the beam by incrementing age */
	if(bullet->dest->is_dead || distance2(&bullet->pos, &bullet->dest->pos) > BULLET_MAX_RANGE)
		bullet->age += idt;
	else
		/* do damage every frame */
		damage_calc(bullet->dest, bullet->damage, idt, bullet->attr);
}

/* @brief updates a pulse bullet */
static void update_pulse(bullet_t *bullet, float dt, int idt)
{
	bullet->age += idt;
	/* bullet lasts a fixed time */
	if(bullet->age > bullet->max_age)
		bullet_destroy(bullet);
	else
		damage_calc(bullet->dest, bullet->damage, idt, bullet->attr);
}

void bullet_update_all(float dt, int idt)
{
	bullet_t *cur;
	Q_FOREACH(cur, &bullet_list, list)
		cur->update(cur, dt, idt);
}

void bullet_draw_all()
{
	bullet_t *cur;
	Q_FOREACH(cur, &bullet_list, list)
		cur->draw(cur);
}
