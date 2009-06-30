#include "controls.h"
#include "state.h"
#include "grid_objs.h"

#define SPACING 40
#define NPOINTS 36
#define PRELIGHT_SIZE 4
#define X_WIDTH 2
#define X_SIZE 20

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
	{ .n = sizeof(p0)/sizeof(*p0), .points = p0 },
	{ .n = sizeof(p1)/sizeof(*p1), .points = p1 },
	{ .n = sizeof(p2)/sizeof(*p2), .points = p2 },
	{ .n = sizeof(p3)/sizeof(*p3), .points = p3 },
	{ .n = sizeof(p4)/sizeof(*p4), .points = p4 },
	{ .n = sizeof(p5)/sizeof(*p5), .points = p5 },
	{ .n = sizeof(p6)/sizeof(*p6), .points = p6 },
	{ .n = sizeof(p7)/sizeof(*p7), .points = p7 },
	{ .n = sizeof(p8)/sizeof(*p8), .points = p8 },
	{ .n = sizeof(p9)/sizeof(*p9), .points = p9 }
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
		glScalef(200.0, 200.0, 0);
		glCallList(DISPLAY_LIST_CIRCLE);
	}
	glPopMatrix();
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

	glNewList(DISPLAY_LIST_GRID, GL_COMPILE);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for(i = -PRELIGHT_SIZE + 1; i < PRELIGHT_SIZE + 1; i++) {
		glVertex2f(i * GRID_SIZE, -PRELIGHT_SIZE * GRID_SIZE);
		glVertex2f(i * GRID_SIZE, (PRELIGHT_SIZE + 1) * GRID_SIZE);
	}
	for(i = -PRELIGHT_SIZE + 1; i < PRELIGHT_SIZE + 1; i++) {
		glVertex2f(-PRELIGHT_SIZE * GRID_SIZE, i * GRID_SIZE);
		glVertex2f((PRELIGHT_SIZE + 1) * GRID_SIZE, i * GRID_SIZE);
	}
	glEnd();
	glEndList();

	glNewList(DISPLAY_LIST_OCCLUDE, GL_COMPILE);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.0, 0.0, 0.0, 0.0);
	glVertex2f(0.0, 0.0);
	glColor4f(0.0, 0.0, 0.0, 1.0);
	for(i = 0; i <= NPOINTS; i++) {
		glVertex2f(PRELIGHT_SIZE * GRID_SIZE * sinf(i * (2 * M_PI / NPOINTS)),
		           PRELIGHT_SIZE * GRID_SIZE * cosf(i * (2 * M_PI / NPOINTS)));
	}
	glEnd();
	glEndList();

	glNewList(DISPLAY_LIST_CIRCLE, GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	for(i = 0; i <= NPOINTS; i++) {
		glVertex2f(sinf(i * (2 * M_PI / NPOINTS)),
		           cosf(i * (2 * M_PI / NPOINTS)));
	}
	glEnd();
	glEndList();

	glNewList(DISPLAY_LIST_X, GL_COMPILE);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0, 0.0);
	glVertex2f(X_WIDTH, 0.0);
	glVertex2f(X_SIZE + X_WIDTH, X_SIZE);
	glVertex2f(X_SIZE + X_WIDTH, X_SIZE + X_WIDTH);
	glVertex2f(X_SIZE, X_SIZE + X_WIDTH);
	glVertex2f(0.0, X_WIDTH);
	glVertex2f(-X_SIZE, X_SIZE + X_WIDTH);
	glVertex2f(-X_SIZE - X_WIDTH, X_SIZE + X_WIDTH);
	glVertex2f(-X_SIZE - X_WIDTH, X_SIZE);
	glVertex2f(-X_WIDTH, 0.0);
	glVertex2f(-X_SIZE - X_WIDTH, -X_SIZE);
	glVertex2f(-X_SIZE - X_WIDTH, -X_SIZE - X_WIDTH);
	glVertex2f(-X_SIZE, -X_SIZE - X_WIDTH);
	glVertex2f(0.0, -X_WIDTH);
	glVertex2f(X_SIZE, -X_SIZE - X_WIDTH);
	glVertex2f(X_SIZE + X_WIDTH, -X_SIZE - X_WIDTH);
	glVertex2f(X_SIZE + X_WIDTH, -X_SIZE);
	glVertex2f(X_WIDTH, 0.0);
	glEnd();
	glEndList();
}

void controls_draw(int x, int y, state_t *state)
{
	draw_prelight_grid(x, y, state);
	draw_scores(state);
}

void controls_click(int x, int y, state_t *state)
{
}
