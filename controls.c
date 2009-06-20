#include "controls.h"
#include "state.h"

#define SPACING 40

float seg1[] = {0.1 ,0.0 , 0.45,0.0 , 0.45,0.1 , 0.1 ,0.1 };
float seg2[] = {0.45,0.1 , 0.55,0.1 , 0.55,0.45, 0.45,0.45};
float seg3[] = {0.45,0.55, 0.55,0.55, 0.55,0.9 , 0.45,0.9 };
float seg4[] = {0.1 ,0.9 , 0.45,0.9 , 0.45,1.0 , 0.1 ,1.0 };
float seg5[] = {0.0 ,0.55, 0.1 ,0.55, 0.1 ,0.9 , 0.0 ,0.9 };
float seg6[] = {0.0 ,0.1 , 0.1 ,0.1 , 0.1 ,0.45, 0.0 ,0.45};
float seg7[] = {0.1 ,0.45, 0.45,0.45, 0.45,0.55, 0.1 ,0.55};

float *p0[] = {seg1, seg2, seg3, seg4, seg5, seg6};
float *p1[] = {seg2, seg3};
float *p2[] = {seg1, seg2, seg4, seg5, seg7};
float *p3[] = {seg1, seg2, seg3, seg4, seg7};
float *p4[] = {seg2, seg3, seg6, seg7};
float *p5[] = {seg1, seg3, seg4, seg6, seg7};
float *p6[] = {seg1, seg3, seg4, seg5, seg6, seg7};
float *p7[] = {seg1, seg2, seg3};
float *p8[] = {seg1, seg2, seg3, seg4, seg5, seg6, seg7};
float *p9[] = {seg1, seg2, seg3, seg4, seg6, seg7};

struct {
	int n;
	float **points;
} glyphs[10] = {
	{ .n = 6, .points = p0 },
	{ .n = 2, .points = p1 },
	{ .n = 5, .points = p2 },
	{ .n = 5, .points = p3 },
	{ .n = 4, .points = p4 },
	{ .n = 5, .points = p5 },
	{ .n = 6, .points = p6 },
	{ .n = 3, .points = p7 },
	{ .n = 7, .points = p8 },
	{ .n = 6, .points = p9 }
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

static void draw_buttons(state_t *state)
{
	int i;

	glBegin(GL_LINES);
	glColor3f(0.5, 0.5, 0.5);
	glVertex2f(SIDEBAR_SIZE, 0.0);
	glVertex2f(SIDEBAR_SIZE, YRES);
	glEnd();

	for(i = 0; i < ATTR_NUM; i++) {
		float offset = i * SPACING;
		glBegin(GL_QUADS);
		glColor3fv(attr_colors[i]);
		glVertex2f(SIDEBAR_SIZE/2 - TOWER_SIZE/2, SIDEBAR_SIZE/2 - TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + TOWER_SIZE/2, SIDEBAR_SIZE/2 - TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + TOWER_SIZE/2, SIDEBAR_SIZE/2 + TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - TOWER_SIZE/2, SIDEBAR_SIZE/2 + TOWER_SIZE/2 + offset);
		glEnd();

		glBegin(GL_LINE_STRIP);
		if(i == state->type_selected)
			glColor4f(1.0, 1.0, 1.0, 1.0);
		else
			glColor4f(0.5, 0.5, 0.5, 1.0);
		glVertex2f(SIDEBAR_SIZE/2 - TOWER_SIZE/2, SIDEBAR_SIZE/2 - TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + TOWER_SIZE/2, SIDEBAR_SIZE/2 - TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 + TOWER_SIZE/2, SIDEBAR_SIZE/2 + TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - TOWER_SIZE/2, SIDEBAR_SIZE/2 + TOWER_SIZE/2 + offset);
		glVertex2f(SIDEBAR_SIZE/2 - TOWER_SIZE/2, SIDEBAR_SIZE/2 - TOWER_SIZE/2 + offset);
		glEnd();
	}
}

void controls_init(void)
{
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
}

void controls_draw(state_t *state)
{
	draw_buttons(state);
	draw_scores(state);
}

void controls_click(int x, int y, state_t *state)
{
	if(x > SIDEBAR_SIZE/2 - TOWER_SIZE/2 && x < SIDEBAR_SIZE/2 + TOWER_SIZE/2) {
		int ynorm, yoff, ypos;
		ynorm = y - SIDEBAR_SIZE/2 + TOWER_SIZE/2;
		yoff = ynorm % SPACING;
		ypos = ynorm / SPACING;
		if(yoff <= TOWER_SIZE && ypos < ATTR_NUM)
			state->type_selected = ypos;
	}
}
