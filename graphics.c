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
#include "tower_types.h"

#define VA_LEN(x) (sizeof(x) / (2 * sizeof(float)))
#define DL_TYPE(x) (DISPLAY_LIST_TOWER_BASE + (x))

#define NPOINTS 64       /* number of points in the prelight and range circles */
#define PRELIGHT_SIZE 3  /* diameter of prelight grid */
#define X_WIDTH 2        /* width of cannot-place-here X */
#define X_SIZE 20        /* size of X */

static float tower_points[] = {
	-TOWER_SIZE/2, -TOWER_SIZE/2,  -TOWER_SIZE/2,  TOWER_SIZE/2,
	 TOWER_SIZE/2,  TOWER_SIZE/2,   TOWER_SIZE/2, -TOWER_SIZE/2,
};

static float noob_points[] = {
	-NOOB_SIZE/2, -NOOB_SIZE/2,  -NOOB_SIZE/2,  NOOB_SIZE/2,
	 NOOB_SIZE/2,  NOOB_SIZE/2,   NOOB_SIZE/2, -NOOB_SIZE/2
};

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

/* @brief vertex array for control bar occluder */
static float ctl_points[] = {
	0, LEVEL_BAR - BOT_BAR_BUFFER + 3,  0, YRES,
	XRES, YRES,                         XRES, LEVEL_BAR - BOT_BAR_BUFFER + 3,
	0, SEL_Y - BTN_OFFSET,              0, YRES,
	SEL_X + BTN_OFFSET, YRES,           SEL_X + BTN_OFFSET, SEL_Y - BTN_OFFSET,
};

/* @brief vertex array for the wave indicator box */
static float wave_points[] = {
	0.0, 0.0,                                 0.0, LEVEL_BAR_HEIGHT,
	LEVEL_BAR_WIDTH - 10, LEVEL_BAR_HEIGHT,   LEVEL_BAR_WIDTH - 10, 0.0
};

/* @brief vertex array for armor boxes */
static float armor_points[] = { 0,0, 0,10, 5,10, 5,0 };

/* icons */
static float icon_plasma[] = {-4,-10, 4,-7, -4,-4, 4,-1, -4,2, 4,5, -4,8, 4,11};
static float icon_ltng[]   = {-1,9, 2,-1, -4,1, -1,-9};
static float icon_cw[]     = {-6,7, 5,7, 0,10, 0,-10, -3,10, 4,3, 2,10, -5,3};
static float icon_pulse[]  = {8,0, -9,0, 0,9, 0,-8, 5,6, -6,-5, -6,6, 5,-5};
static float icon_apcr[]   = {10,1, 2,5, -10,5, -10,-4, 1,-4, 10,1};
static float icon_apfsds[] = {-7,2, 8,2, 10,1, 8,-1, -7,-1, -10,-3, -10,4, -7,2};
static float icon_he[]     = {0,-10, 2,-4, 8,-5, 2,0, 8,7, 1,3, 0,10, -2,4, -8,4, -3,2, -8,-6, -3,-3, 0,-10};
static float icon_heat[]   = {2,12, 2,5, -8,8, -1,2, -9,0, -1,-2, -8,-8, 2,-5, 2,-12, 2,-2, 10,0, 2,2, 2,12};
static float icon_area[]   = {9,0, 2,2, 0,9, -2,2, -9,0, -2,-2, 0,-9, 2,-2, 9,0, 6,6, 0,9, -6,6, -9,0, -6,-6, 0,-9, 6,-6, 9,0};
static float icon_basic[]  = {0,4, -4,0, 0,-4, 4,0, 0,4};

static float icon_du_points[] = {-10,-10, -10,10, 10,-10, 10,10, -3,-3, -3,3, 3,-3, 3,3};
static GLubyte icon_du[] = {0,5, 5,3, 3,6, 6,0, 1,4, 4,2, 2,7, 7,1};

static struct {
	int size;
	GLenum type;
	float *list;
} lists[DISPLAY_LIST_TOWER_END + 1] = {
	[0] = { 0, 0, NULL },
	[DISPLAY_LIST_TOWER] = { VA_LEN(tower_points), GL_QUADS,        tower_points },
	[DISPLAY_LIST_NOOB]  = { VA_LEN(noob_points),  GL_QUADS,        noob_points },
	[DISPLAY_LIST_X]     = { VA_LEN(x_points),     GL_TRIANGLE_FAN, x_points },
	[DISPLAY_LIST_WAVE]  = { VA_LEN(wave_points),  GL_LINE_LOOP,    wave_points },
	[DISPLAY_LIST_CTRL]  = { VA_LEN(ctl_points),   GL_QUADS,        ctl_points },

	[DISPLAY_LIST_HAS_ARMOR] = { VA_LEN(armor_points), GL_QUADS,     armor_points },
	[DISPLAY_LIST_NO_ARMOR]  = { VA_LEN(armor_points), GL_LINE_LOOP, armor_points },

	[DL_TYPE(TT_PLASMA)] = { VA_LEN(icon_plasma), GL_LINE_STRIP, icon_plasma },
	[DL_TYPE(TT_LTNG)]   = { VA_LEN(icon_ltng),   GL_LINE_STRIP, icon_ltng },
	[DL_TYPE(TT_CW)]     = { VA_LEN(icon_cw),     GL_LINES,      icon_cw },
	[DL_TYPE(TT_PULSE)]  = { VA_LEN(icon_pulse),  GL_LINES,      icon_pulse },
	[DL_TYPE(TT_APCR)]   = { VA_LEN(icon_apcr),   GL_LINE_STRIP, icon_apcr },
	[DL_TYPE(TT_APFSDS)] = { VA_LEN(icon_apfsds), GL_LINE_STRIP, icon_apfsds },
	[DL_TYPE(TT_DU)]     = { sizeof(icon_du),     GL_LINES,      icon_du_points },
	[DL_TYPE(TT_HE)]     = { VA_LEN(icon_he),     GL_LINE_STRIP, icon_he },
	[DL_TYPE(TT_HEAT)]   = { VA_LEN(icon_heat),   GL_LINE_STRIP, icon_heat },
	[DL_TYPE(TT_AREA)]   = { VA_LEN(icon_area),   GL_LINE_STRIP, icon_area },
	[DL_TYPE(TT_BASIC)]  = { VA_LEN(icon_basic),  GL_LINE_STRIP, icon_basic }
};

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

	/* compile generic circle */
	glNewList(DISPLAY_LIST_CIRCLE, GL_COMPILE);
	for(i = 0; i <= NPOINTS; i++) {
		glVertex2f(sinf(i * (2 * M_PI / NPOINTS)),
		           cosf(i * (2 * M_PI / NPOINTS)));
	}
	glEndList();

	/* compile glow effect */
	glNewList(DISPLAY_LIST_GLOW, GL_COMPILE);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, 0.0, 0.0);
	glColor4f(0.0, 0.0, 0.0, 0.0);
	glCallList(DISPLAY_LIST_CIRCLE);
	glEnd();
	glEndList();
}

static void init_vertex_lists()
{
	int i;

	for(i = DISPLAY_LIST_BEGIN; i <= DISPLAY_LIST_TOWER_END; i++) {
		glVertexPointer(2, GL_FLOAT, 0, lists[i].list);
		glNewList(i, GL_COMPILE);
		if(i == DL_TYPE(TT_DU))
			glDrawElements(lists[i].type, lists[i].size,
			               GL_UNSIGNED_BYTE, icon_du);
		else
			glDrawArrays(lists[i].type, 0, lists[i].size);
		glEndList();
	}
}

/* @brief compiles a whole lot of display lists */
void graphics_init(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	init_prelight();
	init_vertex_lists();
	glDisableClientState(GL_VERTEX_ARRAY);
}
