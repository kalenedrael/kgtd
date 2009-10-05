/* @file graphics.c
 * @brief Initializes graphics (compiles display lists)
 *
 * note: the path display list is not compiled here since it is done each time
 * the path is loaded
 *
 * top part is mostly vertex data of some sort; separating data and code makes
 * for much less generated code
 */

#include "globals.h"
#include "graphics.h"
#include "controls.h"
#include "attr.h"

/* icons */
static float icon_plasma[] = {-4,-10, 4,-7, -4,-4, 4,-1, -4,2, 4,5, -4,8, 4,11};
static float icon_ltng[]   = {-1,9, 2,-1, -4,1, -1,-9};
static float icon_cw[]     = {-6,7, 5,7, 0,10, 0,-10, -3,10, 4,3, 2,10, -5,3};
static float icon_pulse[]  = {8,0, -9,0, 0,9, 0,-8, 5,6, -6,-5, -6,6, 5,-5};
static float icon_apcr[]   = {10,1, 2,5, -10,5, -10,-4, 1,-4, 10,1};
static float icon_apfsds[] = {-7,2, 8,2, 10,1, 8,-1, -7,-1, -10,-3, -10,4, -7,2};
static float icon_he[]     = {0,-10, 2,-4, 8,-5, 2,0, 8,7, 1,3, 0,10, -2,4, -8,4, -3,2, -8,-6, -3,-3, 0,-10};
static float icon_heat[]   = {2,12, 2,5, -8,8, -1,2, -9,0, -1,-2, -8,-8, 2,-5, 2,-12, 2,-2, 10,0, 2,2, 2,12};
static float icon_hesh[]   = {5,-12, 5,12, 5,5, -5,8, 2,2, -6,0, 2,-2, -5,-8, 5,-5, 5,-12};
static float icon_basic[]  = {0,4, -4,0, 0,-4, 4,0, 0,4};

static float icon_du_points[] = {-10,-10, -10,10, 10,-10, 10,10, -3,-3, -3,3, 3,-3, 3,3};
static GLubyte icon_du[] = {0,5, 5,3, 3,6, 6,0, 1,4, 4,2, 2,7, 7,1};

#define VA_LEN(x) (sizeof(x) / (2 * sizeof(float)))

static struct {
	int size;
	GLenum type;
	float *icon;
} icons[ATTR_NUM] = {
	[ATTR_PLASMA] = { VA_LEN(icon_plasma), GL_LINE_STRIP, icon_plasma },
	[ATTR_LTNG]   = { VA_LEN(icon_ltng),   GL_LINE_STRIP, icon_ltng },
	[ATTR_CW]     = { VA_LEN(icon_cw),     GL_LINES,      icon_cw },
	[ATTR_PULSE]  = { VA_LEN(icon_pulse),  GL_LINES,      icon_pulse },
	[ATTR_APCR]   = { VA_LEN(icon_apcr),   GL_LINE_STRIP, icon_apcr },
	[ATTR_APFSDS] = { VA_LEN(icon_apfsds), GL_LINE_STRIP, icon_apfsds },
	[ATTR_DU]     = { sizeof(icon_du),     GL_LINES,      icon_du_points},
	[ATTR_HE]     = { VA_LEN(icon_he),     GL_LINE_STRIP, icon_he },
	[ATTR_HEAT]   = { VA_LEN(icon_heat),   GL_LINE_STRIP, icon_heat },
	[ATTR_HESH]   = { VA_LEN(icon_hesh),   GL_LINE_STRIP, icon_hesh },
	[ATTR_BASIC]  = { VA_LEN(icon_basic),  GL_LINE_STRIP, icon_basic }
};

#define NPOINTS 64       /* number of points in the prelight and range circles */
#define PRELIGHT_SIZE 4  /* diameter of prelight grid */
#define X_WIDTH 2        /* width of cannot-place-here X */
#define X_SIZE 20        /* size of X */

/* @brief vertex array for the X */
static float x_points[] = {
	0.0, 0.0,                    X_WIDTH, 0.0,
	X_SIZE + X_WIDTH, X_SIZE,    X_SIZE + X_WIDTH, X_SIZE + X_WIDTH,
	X_SIZE, X_SIZE + X_WIDTH,    0.0, X_WIDTH,
	-X_SIZE, X_SIZE + X_WIDTH,   -X_SIZE - X_WIDTH, X_SIZE + X_WIDTH,
	-X_SIZE - X_WIDTH, X_SIZE,   -X_WIDTH, 0.0,
	-X_SIZE - X_WIDTH, -X_SIZE,  -X_SIZE - X_WIDTH, -X_SIZE - X_WIDTH,
	-X_SIZE, -X_SIZE - X_WIDTH,  0.0, -X_WIDTH,
	X_SIZE, -X_SIZE - X_WIDTH,   X_SIZE + X_WIDTH, -X_SIZE - X_WIDTH,
	X_SIZE + X_WIDTH, -X_SIZE,   X_WIDTH, 0.0
};

/* @brief vertex array for control bar */
static float control_points[] = {
	0, LEVEL_BAR - BOT_BAR_BUFFER + 3,  0, YRES,
	XRES, YRES,                         XRES, LEVEL_BAR - BOT_BAR_BUFFER + 3,
	0, SEL_Y - BTN_OFFSET,              0, YRES,
	SEL_X + BTN_OFFSET, YRES,           SEL_X + BTN_OFFSET, SEL_Y - BTN_OFFSET,
};
/* @brief vertex array for the wave indicator box */
static float wave_points[] = {
	0.0, 0.0,   0.0, LEVEL_BAR_HEIGHT, LEVEL_BAR_WIDTH - 10,
	LEVEL_BAR_HEIGHT, LEVEL_BAR_WIDTH - 10, 0.0, 0.0, 0.0
};
static float armor_points[] = { 0,0, 0,10, 5,10, 5,0, 0,0 };

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
	glVertexPointer(2, GL_FLOAT, 0, x_points);
	glDrawArrays(GL_TRIANGLE_FAN, 0, VA_LEN(x_points));
	glEndList();
}

/* @brief compiles the display lists for towers*/
static void init_tower()
{
	int i;

	glNewList(DISPLAY_LIST_TOWER, GL_COMPILE);
	glBegin(GL_QUADS);
	glVertex2f(-TOWER_SIZE/2, -TOWER_SIZE/2);
	glVertex2f(-TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2,  TOWER_SIZE/2);
	glVertex2f( TOWER_SIZE/2, -TOWER_SIZE/2);
	glEnd();
	glEndList();

	for(i = 0; i < ATTR_NUM; i++) {
		glVertexPointer(2, GL_FLOAT, 0, icons[i].icon);
		glNewList(DISPLAY_LIST_TOWER_BASE + i, GL_COMPILE);
		if(i == ATTR_DU)
			glDrawElements(icons[i].type, icons[i].size,
			               GL_UNSIGNED_BYTE, icon_du);
		else
			glDrawArrays(icons[i].type, 0, icons[i].size);
		glEndList();
	}
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

/* @brief compiles display list for control bar occluder */
static void init_controls()
{
	glNewList(DISPLAY_LIST_CONTROLS, GL_COMPILE);
	glColor3f(0.0, 0.0, 0.0);
	glVertexPointer(2, GL_FLOAT, 0, control_points);
	glDrawArrays(GL_QUADS, 0, VA_LEN(control_points));
	glEndList();

	glNewList(DISPLAY_LIST_WAVE, GL_COMPILE);
	glVertexPointer(2, GL_FLOAT, 0, wave_points);
	glDrawArrays(GL_LINE_STRIP, 0, VA_LEN(wave_points));
	glEndList();

	glNewList(DISPLAY_LIST_HAS_ARMOR, GL_COMPILE);
	glVertexPointer(2, GL_FLOAT, 0, armor_points);
	glDrawArrays(GL_QUADS, 0, VA_LEN(armor_points) - 1);
	glEndList();

	glNewList(DISPLAY_LIST_NO_ARMOR, GL_COMPILE);
	glDrawArrays(GL_LINE_STRIP, 0, VA_LEN(armor_points));
	glEndList();
}

/* @brief compiles a whole lot of display lists */
void graphics_init(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	init_prelight();
	init_tower();
	init_noob();
	init_controls();
	glDisableClientState(GL_VERTEX_ARRAY);
}
