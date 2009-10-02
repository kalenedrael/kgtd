/* @file controls.c
 * @brief I hate user interfaces
 */

#include "controls.h"
#include "state.h"
#include "grid_objs.h"
#include "text.h"

typedef struct sel_t {
	attr_t attr;
	unsigned short unlocked;
	unsigned short upgrades;
	struct sel_t *dep;
} sel_t;

#define UPG_UP 1
#define UPG_DIAG 2
#define UPG_LEFT 4
#define SEL_BOX_SIZE 20
#define BUF_SIZE 256

#define in_sel_box(x,y) ((x) < SEL_X && (y) > SEL_Y)
#define in_main_area(x,y) (((x) > SEL_X + SEL_BUFFER || (y) < SEL_Y - \
                           SEL_BUFFER) && (y) < BOT_BAR - BOT_BAR_BUFFER)

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

	if(state->selected == ATTR_NONE)
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
	glColor3fv(attr_colors[state->selected]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	if(grid[ay/GRID_SIZE][ax/GRID_SIZE].type != GRID_TYPE_NONE) {
		glCallList(DISPLAY_LIST_X);
	}
	else {
		glCallList(DISPLAY_LIST_TOWER);
		glColor3f(1.0, 1.0, 1.0);
		glCallList(DISPLAY_LIST_TOWER_BASE + state->selected);
		glScalef(150.0, 150.0, 0);
		glCallList(DISPLAY_LIST_CIRCLE);
	}
	glPopMatrix();
}

/* @brief draw one selector button */
static void sel_draw_one(sel_t *cur, int i, int j, attr_t selected)
{
	int x = (i + 1) * BTN_SIZE;
	int y = YRES - (j + 1) * BTN_SIZE;

	if(cur->attr == ATTR_NONE)
		return;

	/* draw if active or can be upgraded next */
	if(cur->unlocked || cur->dep->unlocked) {
		glPushMatrix();
		glTranslatef(x - TOWER_SIZE/2, y + TOWER_SIZE/2, 0);
		glColor3f(1.0, 1.0, 1.0);
		if(cur->unlocked) {
			glCallList(DISPLAY_LIST_TOWER_BASE + cur->attr);
			glColor3fv(attr_colors[cur->attr]);
			glCallList(DISPLAY_LIST_TOWER);
		}
		else {
			glCallList(DISPLAY_LIST_TOWER_BASE + cur->attr);
		}
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
			glVertex2f(x, y + TOWER_SIZE/2);
			glColor3fv(attr_colors[sel_arr[j][i+1].attr]);
			glVertex2f(x + BTN_OFFSET, y + TOWER_SIZE/2);
		}
		if(cur->upgrades & UPG_UP) {
			glColor3fv(attr_colors[cur->attr]);
			glVertex2f(x - TOWER_SIZE/2, y);
			glColor3fv(attr_colors[sel_arr[j+1][i].attr]);
			glVertex2f(x - TOWER_SIZE/2, y - BTN_OFFSET);
		}
		if(cur->upgrades & UPG_DIAG) {
			glColor3fv(attr_colors[cur->attr]);
			glVertex2f(x, y);
			glColor3fv(attr_colors[sel_arr[j+1][i+1].attr]);
			glVertex2f(x + BTN_OFFSET, y - BTN_OFFSET);
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
			sel_draw_one(&sel_arr[j][i], i, j, state->selected);

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
		state->selected = cur->attr;
	else if(cur->dep && cur->dep->unlocked)
		cur->unlocked = 1;

	return;
}

static void bar_draw(int x, int y, state_t *state)
{
	char buf[BUF_SIZE];

	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_LINES);
	glVertex2f(SEL_X + BTN_OFFSET, BOT_BAR);
	glVertex2f(XRES, BOT_BAR);
	glEnd();

	buf[0] = '\0';
	glColor3f(0.9, 0.9, 0.9);
	if(in_main_area(x, y)) {
		if(state->selected != ATTR_NONE)
			snprintf(buf, sizeof(buf), "Click to place %s tower",
			         attr_dscr[state->selected]);
		else if(grid[y/GRID_SIZE][x/GRID_SIZE].type == GRID_TYPE_TOWER)
			strcpy(buf, "Click to increase power supplied, "
			            "right click to decrease");
		else
			strcpy(buf, "Select a tower type from the lower left");
	}
	else if(in_sel_box(x, y)) {
		int sy = YRES - y;
		if(x % BTN_SIZE > BTN_OFFSET && sy % BTN_SIZE > BTN_OFFSET) {
			sel_t *sel = &sel_arr[sy / BTN_SIZE][x / BTN_SIZE];
			if(sel->unlocked)
				snprintf(buf, sizeof(buf), "Click to select %s tower",
				         attr_dscr[sel->attr]);
			else if(sel->dep && sel->dep->unlocked)
				snprintf(buf, sizeof(buf), "Click to unlock %s tower",
				         attr_dscr[sel->attr]);
		}
	}
	text_draw(buf, 184, 550);
	snprintf(buf, sizeof(buf), "Power used: %d/%d", state->power_used,
	                                                state->max_power);
	text_draw(buf, 184, 576);
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

void controls_draw(state_t *state)
{
	int x, y;

	SDL_GetMouseState(&x, &y);
	if(in_main_area(x, y))
		draw_prelight_grid(x, y, state);
	draw_scores(state);
	bar_draw(x, y, state);
	sel_draw(state);
}

void controls_update(int dt, state_t *state)
{
	int x, y, dp = 0, power;
	Uint8 btn = SDL_GetMouseState(&x, &y);
	tower_t *tower = &grid[y/GRID_SIZE][x/GRID_SIZE].t;

	if(state->selected != ATTR_NONE)
		return;
	if(tower->type != GRID_TYPE_TOWER)
		return;

	power = tower->power;
	if(btn & SDL_BUTTON(1))
		dp = power + dt/2 > TOWER_MAX_PWR ? TOWER_MAX_PWR - power : dt/2;
	else if(btn & SDL_BUTTON(3))
		dp = power < dt/2 ? -power : -dt/2;

	if(state->power_used + dp > state->max_power)
		dp = state->max_power - state->power_used;

	tower->power += dp;
	state->power_used += dp;
}

void controls_click(SDL_MouseButtonEvent *ev, state_t *state)
{
	if(ev->button == SDL_BUTTON_RIGHT) {
		state->selected = ATTR_NONE;
	}
	else if(in_main_area(ev->x, ev->y)) {
		int gx = ev->x/GRID_SIZE;
		int gy = ev->y/GRID_SIZE;
		if(ev->button == SDL_BUTTON_LEFT && state->selected != ATTR_NONE) {
			int power = state->max_power - state->power_used;
			power = power > 100 ? 100 : power;
			tower_t *tower = tower_new(gx, gy, power, state->selected);
			if(tower != NULL) {
				state->towers++;
				state->power_used += tower->power;
			}
		}
	}
	else if(in_sel_box(ev->x, ev->y)) {
		int x = ev->x;
		int y = YRES - ev->y;
		if(x % BTN_SIZE > BTN_OFFSET && y % BTN_SIZE > BTN_OFFSET)
			sel_click(x / BTN_SIZE, y / BTN_SIZE, state);
	}
}
