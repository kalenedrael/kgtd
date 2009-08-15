/* @file graphics.c
 * @brief Initializes graphics (compiles display lists)
 *
 * note: the path display list is not compiled here since it is done each time
 * the path is loaded
 */

#include "globals.h"
#include "graphics.h"
#include "attr.h"

/* @brief 7-seg segments - glyphs are meant to be 1 x 0.6 units */
static float seg1[] = {0.1 ,0.0 , 0.45,0.0 , 0.45,0.1 , 0.1 ,0.1 };
static float seg2[] = {0.45,0.1 , 0.55,0.1 , 0.55,0.45, 0.45,0.45};
static float seg3[] = {0.45,0.55, 0.55,0.55, 0.55,0.9 , 0.45,0.9 };
static float seg4[] = {0.1 ,0.9 , 0.45,0.9 , 0.45,1.0 , 0.1 ,1.0 };
static float seg5[] = {0.0 ,0.55, 0.1 ,0.55, 0.1 ,0.9 , 0.0 ,0.9 };
static float seg6[] = {0.0 ,0.1 , 0.1 ,0.1 , 0.1 ,0.45, 0.0 ,0.45};
static float seg7[] = {0.1 ,0.45, 0.45,0.45, 0.45,0.55, 0.1 ,0.55};

/* @brief Numbers in terms of segments */
static float *p0[] = {seg1, seg2, seg3, seg4, seg5, seg6};
static float *p1[] = {seg2, seg3};
static float *p2[] = {seg1, seg2, seg4, seg5, seg7};
static float *p3[] = {seg1, seg2, seg3, seg4, seg7};
static float *p4[] = {seg2, seg3, seg6, seg7};
static float *p5[] = {seg1, seg3, seg4, seg6, seg7};
static float *p6[] = {seg1, seg3, seg4, seg5, seg6, seg7};
static float *p7[] = {seg1, seg2, seg3};
static float *p8[] = {seg1, seg2, seg3, seg4, seg5, seg6, seg7};
static float *p9[] = {seg1, seg2, seg3, seg4, seg6, seg7};

static struct {
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

static float icon_plasma[] = {-4,-10, 4,-7, -4,-4, 4,-1, -4,2, 4,5, -4,8, 4,11};
static float icon_ltng[]   = {-1,9, 2,-1, -4,1, -1,-9};
static float icon_cw[]     = {-6,7, 5,7, 0,10, 0,-10, -3,10, 4,3, 2,10, -5,3};
static float icon_pulse[]  = {8,0, -9,0, 0,9, 0,-8, 5,6, -6,-5, -6,6, 5,-5};
static float icon_apcr[]   = {10,1, 2,5, -10,5, -10,-4, 1,-4, 10,1};
static float icon_apfsds[] = {-7,2, 8,2, 10,1, 8,-1, -7,-1, -10,-3, -10,4, -7,2};
static float icon_he[]     = {0,-10, 2,-4, 8,-5, 2,0, 8,7, 1,3, 0,10, -2,4, -8,4,
                              -3,2, -8,-6, -3,-3, 0,-10};
static float icon_heat[]   = {2,12, 2,5, -8,8, -1,2, -9,0, -1,-2, -8,-8, 2,-5,
                              2,-12, 2,-2, 10,0, 2,2, 2,12};
static float icon_hesh[]   = {5,-12, 5,12, 5,5, -5,8, 2,2, -6,0, 2,-2, -5,-8,
                              5,-5, 5,-12};
static float icon_basic[]  = {0,4, -4,0, 0,-4, 4,0, 0,4};

static float icon_du[] = {-10,-10, -10,10, 10,-10, 10,10, -3,-3, -3,3, 3,-3, 3,3};
static GLubyte icon_du_points[] = {0,5, 5,3, 3,6, 6,0, 1,4, 4,2, 2,7, 7,1};

#define NPOINTS 64       /* number of points in the prelight and range circles */
#define PRELIGHT_SIZE 4  /* diameter of prelight grid */
#define X_WIDTH 2        /* width of cannot-place-here X */
#define X_SIZE 20        /* size of X */

/* @brief compile number glyphs */
static void init_numbers()
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

/* @brief compile prelight stuff (circle, prelight grid) */
static void init_prelight()
{
	int i;

	/* compile prelight grid */
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

	/* compile prelight occlusion thing */
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

	/* compile range circle */
	glNewList(DISPLAY_LIST_CIRCLE, GL_COMPILE);
	glBegin(GL_LINE_STRIP);
	for(i = 0; i <= NPOINTS; i++) {
		glVertex2f(sinf(i * (2 * M_PI / NPOINTS)),
		           cosf(i * (2 * M_PI / NPOINTS)));
	}
	glEnd();
	glEndList();

	/* compile the X */
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

/* @brief compiles the display lists for towers*/
static void init_tower()
{
	glNewList(DISPLAY_LIST_TOWER, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(-TOWER_SIZE/2, -TOWER_SIZE/2);
	glVertex2f(-TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2, -TOWER_SIZE/2);
	glEnd();
	glEndList();

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, icon_plasma);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_ENERGY_PARTICLE_PLASMA, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_plasma) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_ltng);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_ENERGY_PARTICLE_LIGHTNING, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_ltng) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_cw);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_ENERGY_LASER_CW, GL_COMPILE);
	glDrawArrays(GL_LINES, 0, sizeof(icon_cw) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_pulse);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_ENERGY_LASER_PULSE, GL_COMPILE);
	glDrawArrays(GL_LINES, 0, sizeof(icon_pulse) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_apcr);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_MASS_KINETIC_APCR, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_apcr) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_apfsds);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_MASS_KINETIC_APFSDS, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_apfsds) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_du);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_MASS_KINETIC_DU, GL_COMPILE);
	glDrawElements(GL_LINES, sizeof(icon_du_points), GL_UNSIGNED_BYTE,
	               icon_du_points);
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_he);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_MASS_EXPLOSIVE_HE, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_he) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_heat);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_MASS_EXPLOSIVE_HEAT, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_heat) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_hesh);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_MASS_EXPLOSIVE_HESH, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_hesh) / (2 * sizeof(float)));
	glEndList();

	glVertexPointer(2, GL_FLOAT, 0, icon_basic);
	glNewList(DISPLAY_LIST_TOWER_BASE + ATTR_BASIC, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(icon_basic) / (2 * sizeof(float)));
	glEndList();

	glDisableClientState(GL_VERTEX_ARRAY);
}

/* @brief compiles the display list for a noob */
static void init_noob()
{
	glNewList(DISPLAY_LIST_NOOB, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(-NOOB_SIZE/2, -NOOB_SIZE/2);
	glVertex2f(-NOOB_SIZE/2,  NOOB_SIZE/2);
	glVertex2f( NOOB_SIZE/2,  NOOB_SIZE/2);
	glVertex2f( NOOB_SIZE/2, -NOOB_SIZE/2);
	glEnd();
	glEndList();
}

/* @brief compiles a whole lot of display lists */
void graphics_init(void)
{
	init_numbers();
	init_prelight();
	init_tower();
	init_noob();
}
