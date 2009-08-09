#include "controls.h"
#include "state.h"
#include "grid_objs.h"

#define SELECTOR_X 160
#define SELECTOR_Y (YRES - 160)

static void draw_num(unsigned int num, float x, float y, float scale)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1.0);
	do {
		glTranslatef(-0.9, 0, 0);
		glCallList(DISPLAY_LIST_NUM_BASE + num % 10);
		num = num / 10;
	} while (num);
	glPopMatrix();
}

static void draw_scores(state_t *state)
{
	glColor4f(1.0, 1.0, 1.0, 0.5);
	draw_num(state->total_noobs, XRES, 0.0, 30.0);
	glColor4f(0.1, 1.0, 0.1, 1.0);
	draw_num(state->kills, XRES, 40.0, 30.0);
	glColor3f(1.0, 0.1, 0.1);
	draw_num(state->leaks, XRES, 80.0, 30.0);
}

static void draw_prelight_grid(int x, int y, state_t *state)
{
	int ax, ay;

	if(state->type_selected == ATTR_NONE)
		return;

	ax = (x / GRID_SIZE) * GRID_SIZE;
	ay = (y / GRID_SIZE) * GRID_SIZE;
	if(ax > GRID_X * GRID_SIZE || ay > GRID_Y * GRID_SIZE)
		return;

	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glCallList(DISPLAY_LIST_OCCLUDE);
	glPopMatrix();
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTranslatef(ax, ay, 0);
	glCallList(DISPLAY_LIST_GRID);
	glPopMatrix();
	glStencilFunc(GL_ALWAYS, 1, 1);

	glPushMatrix();
	glTranslatef(x, y, 0);
	glCallList(DISPLAY_LIST_OCCLUDE);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(ax + GRID_SIZE/2, ay + GRID_SIZE/2, 0.0);
	glColor3fv(attr_colors[state->type_selected]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	if(grid[ay/GRID_SIZE][ax/GRID_SIZE].type != GRID_TYPE_NONE) {
		glCallList(DISPLAY_LIST_X);
	}
	else {
		glCallList(DISPLAY_LIST_TOWER);
		glScalef(150.0, 150.0, 0);
		glCallList(DISPLAY_LIST_CIRCLE);
	}
	glPopMatrix();
}

static void place_tower(int x, int y, unsigned int power, attr_t attr)
{
	switch(attr) {
	case ATTR_ENERGY_PARTICLE_LIGHTNING:
	case ATTR_ENERGY_LASER_PULSE:
		tower_new_pulse(x, y, power, attr);
		break;
	case ATTR_ENERGY_LASER_CW:
	case ATTR_ENERGY_PARTICLE_PLASMA:
		tower_new_cw(x, y, power, attr);
		break;
	case ATTR_MASS_KINETIC_APCR:
	case ATTR_MASS_KINETIC_APFSDS:
	case ATTR_MASS_KINETIC_DU:
	case ATTR_MASS_EXPLOSIVE_HE:
	case ATTR_MASS_EXPLOSIVE_HEAT:
	case ATTR_MASS_EXPLOSIVE_HESH:
		tower_new_proj(x, y, power, attr);
	case ATTR_NONE:
		break;
	}
}

void controls_init(void)
{
}

void controls_draw(int x, int y, state_t *state)
{
	if(x > SELECTOR_X || y < SELECTOR_Y)
		draw_prelight_grid(x, y, state);
	draw_scores(state);
}

void controls_click(SDL_MouseButtonEvent *ev, state_t *state)
{
	int gx, gy;

	if(ev->x <= SELECTOR_X && ev->y >= SELECTOR_Y)
		return;

	gx = ev->x/GRID_SIZE;
	gy = ev->y/GRID_SIZE;
	if(ev->button == SDL_BUTTON_LEFT && state->type_selected != ATTR_NONE) {
		place_tower(gx, gy, 100, state->type_selected);
		state->type_selected = ATTR_NONE;
	}

	return;
}
