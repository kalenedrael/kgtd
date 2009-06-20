#include "controls.h"
#include "state.h"

#define SPACING 40
/*
struct {
	int n;
	float points[2][8];
} glyphs[] = {
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}},
	{ .n = 2, .points = {{0,0, 1,0, 1,1, 0,1},{2,0, 3,0, 3,1, 2,1}}}
};
*/
static void draw_num(unsigned int num, float x, float y, float scale)
{
/*
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1.0);
	do {
		glTranslatef(-i, 0, 0);
		glCallList(DISPLAY_LIST_NUM_BASE + num % 10);
		i++;
		num = num / 10;
	} while (num);
	glPopMatrix();
*/
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(scale, scale, 1.0);
	for(; num; num = num >> 1) {
		glTranslatef(-1.0, 0.0, 0.0);
		if(num & 1)
			glCallList(DISPLAY_LIST_NUM_BASE);
	}
	glPopMatrix();
}

static void draw_scores(state_t *state)
{
//	draw_num(state->score, 600.0, 10.0, 10.0);
	glColor3f(1.0, 1.0, 1.0);
	draw_num(state->total_noobs, XRES, 0.0, 10.0);
	draw_num(state->kills, XRES, 10.0, 10.0);
	glColor3f(1.0, 0.1, 0.1);
	draw_num(state->leaks, XRES, 20.0, 10.0);
}

static void draw_buttons(state_t *state)
{
	int i;

	glBegin(GL_QUADS);
	glColor3f(0.1, 0.1, 0.1);
	glVertex2f(0.0, 0.0);
	glVertex2f(SIDEBAR_SIZE, 0.0);
	glVertex2f(SIDEBAR_SIZE, YRES);
	glVertex2f(0.0, YRES);
	glEnd();

	for(i = 0; i < ATTR_NUM; i++) {
		float offset = i * SPACING;
		glBegin(GL_QUADS);
		glColor3fv(attr_colors[i]);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glEnd();

		glBegin(GL_LINE_STRIP);
		if(i == state->type_selected)
			glColor3f(1.0, 1.0, 1.0);
		else
			glColor3f(0.4, 0.4, 0.4);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 + GRID_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - GRID_SIZE/2, SIDEBAR_SIZE/2 - GRID_SIZE/2 + offset);
		glEnd();
	}
}

void controls_init(void)
{
/*
	int i, j;

	for(i = 0; i < 10; i++) {
		glNewList(DISPLAY_LIST_NUM_BASE + i, GL_COMPILE);
		glBegin(GL_QUADS);
		for(j = 0; j < glyphs[i].n; j++) {
			glVertex2fv(glyphs[i].points[j]);
			glVertex2fv(glyphs[i].points[j] + 2);
			glVertex2fv(glyphs[i].points[j] + 4);
			glVertex2fv(glyphs[i].points[j] + 6);
		}
		glEnd();
		glEndList();
	}
*/

	glNewList(DISPLAY_LIST_NUM_BASE, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, 1.0);
	glVertex2f(1.0, 1.0);
	glVertex2f(1.0, 0.0);
	glEnd();
	glEndList();
}

void controls_draw(state_t *state)
{
	draw_buttons(state);
	draw_scores(state);
}

void controls_click(int x, int y, state_t *state)
{
	if(x > SIDEBAR_SIZE/2 - GRID_SIZE/2 && x < SIDEBAR_SIZE/2 + GRID_SIZE/2) {
		int ynorm, yoff, ypos;
		ynorm = y - SIDEBAR_SIZE/2 + GRID_SIZE/2;
		yoff = ynorm % SPACING;
		ypos = ynorm / SPACING;
		if(yoff <= GRID_SIZE && ypos < ATTR_NUM) {
			state->type_selected = ypos;
		}
	}
}

