/* @file controls.c
 * @brief I hate user interfaces
 */

#include "controls.h"
#include "state.h"
#include "grid_objs.h"

typedef struct sel_t {
	attr_t attr;
	int unlocked;
	int upgrades;
	struct sel_t *dep;
} sel_t;

#define UPG_UP 1
#define UPG_DIAG 2
#define UPG_LEFT 4
#define SEL_BOX_SIZE 20

/* @brief the tower selector */
static sel_t sel_arr[4][4] = {
	{
		{ ATTR_BASIC, 1, UPG_UP | UPG_DIAG | UPG_LEFT, NULL },
		{ ATTR_MASS_KINETIC_APCR, 0, UPG_LEFT, &sel_arr[0][0] },
		{ ATTR_MASS_KINETIC_APFSDS, 0, UPG_LEFT, &sel_arr[0][1] },
		{ ATTR_MASS_KINETIC_DU, 0, 0, &sel_arr[0][2] }
	},
	{
		{ ATTR_ENERGY_PARTICLE_PLASMA, 0, UPG_UP | UPG_DIAG, &sel_arr[0][0] },
		{ ATTR_MASS_EXPLOSIVE_HE, 0, UPG_LEFT | UPG_DIAG, &sel_arr[0][0] },
		{ ATTR_MASS_EXPLOSIVE_HEAT, 0, 0, &sel_arr[1][1] },
		{ ATTR_NONE, 0, 0, NULL }
	},
	{
		{ ATTR_ENERGY_LASER_PULSE, 0, UPG_UP, &sel_arr[1][0] },
		{ ATTR_ENERGY_PARTICLE_LIGHTNING, 0, 0, &sel_arr[1][0] },
		{ ATTR_MASS_EXPLOSIVE_HESH, 0, 0, &sel_arr[1][1] },
		{ ATTR_NONE, 0, 0, NULL }
	},
	{
		{ ATTR_ENERGY_LASER_CW, 0, 0, &sel_arr[2][0] },
		{ ATTR_NONE, 0, 0, NULL },
		{ ATTR_NONE, 0, 0, NULL },
		{ ATTR_NONE, 0, 0, NULL }
	}
};

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

	/* this whole stencil dance is to draw that fancy lit grid */
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

/* @brief draw one selector button */
static void sel_draw_one(sel_t *cur, int x, int y, attr_t selected)
{
	if(cur->attr == ATTR_NONE)
		return;
	/* draw if active or can be upgraded next */
	if(cur->unlocked || cur->dep->unlocked) {
		glPushMatrix();
		if(cur->unlocked)
			glColor3fv(attr_colors[cur->attr]);
		else
			glColor3f(0.0, 0.0, 0.0);
		glTranslatef(x - TOWER_SIZE/2, YRES - y + TOWER_SIZE/2, 0);
		glCallList(DISPLAY_LIST_TOWER);
		if(cur->attr == selected) {
			glBegin(GL_LINE_STRIP);
			glVertex2f( SEL_BOX_SIZE,  SEL_BOX_SIZE);
			glVertex2f(-SEL_BOX_SIZE,  SEL_BOX_SIZE);
			glVertex2f(-SEL_BOX_SIZE, -SEL_BOX_SIZE);
			glVertex2f( SEL_BOX_SIZE, -SEL_BOX_SIZE);
			glVertex2f( SEL_BOX_SIZE,  SEL_BOX_SIZE);
			glEnd();
		}
		glPopMatrix();
	}
	/* draw the upgrade lines */
	if(cur->unlocked) {
		glBegin(GL_LINES);
		if(cur->upgrades & UPG_LEFT) {
			glVertex2f(x, YRES - y + TOWER_SIZE/2);
			glVertex2f(x + BTN_OFFSET, YRES - y + TOWER_SIZE/2);
		}
		if(cur->upgrades & UPG_UP) {
			glVertex2f(x - TOWER_SIZE/2, YRES - y);
			glVertex2f(x - TOWER_SIZE/2, YRES - y - BTN_OFFSET);
		}
		if(cur->upgrades & UPG_DIAG) {
			glVertex2f(x, YRES - y);
			glVertex2f(x + BTN_OFFSET, YRES - y - BTN_OFFSET);
		}
		glEnd();
	}
}

/* @brief draw the selectors and the box around them */
static void sel_draw(state_t *state)
{
	int i, j;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			sel_draw_one(&sel_arr[j][i], (i + 1) * BTN_SIZE,
			             (j + 1) * BTN_SIZE, state->type_selected);

	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, SEL_Y - BTN_OFFSET);
	glVertex2f(SEL_X + BTN_OFFSET, SEL_Y - BTN_OFFSET);
	glVertex2f(SEL_X + BTN_OFFSET, YRES);
	glEnd();
}

static void sel_click(int x, int y, state_t *state)
{
	sel_t *cur = &sel_arr[y][x];

	if(cur->unlocked)
		state->type_selected = cur->attr;
	else if(cur->dep->unlocked)
		cur->unlocked = 1;

	return;
}

static void bar_draw(state_t *state)
{
	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_LINES);
	glVertex2f(SEL_X + BTN_OFFSET, BOT_BAR);
	glVertex2f(XRES, BOT_BAR);
	glEnd();
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
	case ATTR_BASIC:
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
	int i, j;

	/* initialize selectors */
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			sel_arr[i][j].unlocked = 0;

	sel_arr[0][0].unlocked = 1;
}

void controls_draw(int x, int y, state_t *state)
{
	if((x > SEL_X + SEL_BUFFER || y < SEL_Y - SEL_BUFFER) &&
	   y < BOT_BAR - BOT_BAR_BUFFER)
		draw_prelight_grid(x, y, state);
	draw_scores(state);
	bar_draw(state);
	sel_draw(state);
}

/* @brief mouse event dispatcher... might want an improved architecture later */
void controls_click(SDL_MouseButtonEvent *ev, state_t *state)
{
	if(ev->button == SDL_BUTTON_RIGHT) {
		state->type_selected = ATTR_NONE;
	}
	else if((ev->x > SEL_X + SEL_BUFFER || ev->y < SEL_Y - SEL_BUFFER) &&
	        ev->y < BOT_BAR - BOT_BAR_BUFFER) {
		int gx = ev->x/GRID_SIZE;
		int gy = ev->y/GRID_SIZE;
		if(ev->button == SDL_BUTTON_LEFT && state->type_selected != ATTR_NONE)
			place_tower(gx, gy, 100, state->type_selected);
	}
	else if(ev->x < SEL_X && ev->y > SEL_Y) {
		int x = ev->x;
		int y = YRES - ev->y;
		if(x % BTN_SIZE > BTN_OFFSET && y % BTN_SIZE > BTN_OFFSET)
			sel_click(x / BTN_SIZE, y / BTN_SIZE, state);
	}
	else if(ev->y > BOT_BAR) {
	}
}
