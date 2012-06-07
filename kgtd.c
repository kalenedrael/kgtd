#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"
#include "grid_objs.h"
#include "state.h"
#include "controls.h"
#include "noob.h"
#include "bullet.h"
#include "level.h"
#include "graphics.h"
#include "text.h"

/* function prototypes */
static void draw();
static void update();
static void reset();

/* SDL stuff */
static SDL_Surface *screen;

/* timekeeper */
static int oldtime;
static int need_draw;

/* game state */
state_t kgtd_state;

static void handle_event(SDL_Event *ev)
{
	switch(ev->type) {
	case SDL_QUIT:
		exit(0);
	case SDL_MOUSEBUTTONUP:
		controls_click(&ev->button, &kgtd_state);
		return;
	case SDL_USEREVENT: /* update event */
		update();
		need_draw = 1;
	default:
		return;
	}
}

static Uint32 timer_cb(Uint32 x, void* p)
{
	SDL_Event tev;
	tev.user.type = SDL_USEREVENT;
	SDL_PushEvent(&tev);
	return x;
}

static void update(void)
{
	int newtime = SDL_GetTicks();
	int idt = newtime - oldtime;
	float dt = (float)idt / 1000.0;

	noob_update_all(dt, idt, &kgtd_state);
	bullet_update_all(dt, idt);
	tower_update_all(idt);
	controls_update(idt, &kgtd_state);
	level_update(idt, &kgtd_state);

	oldtime = newtime;
}

static void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
	        GL_STENCIL_BUFFER_BIT);

	controls_draw(&kgtd_state);
	path_draw_all(&kgtd_state);
	noob_draw_all();
	tower_draw_all();
	bullet_draw_all();

	SDL_GL_SwapBuffers();
}

static void reset(void)
{
	grid_init();
	bullet_init();
	noob_init();
	tower_init();
	state_reset(&kgtd_state);
	level_init(&kgtd_state);
}

static void init(void)
{
	/* set up SDL */
	screen = NULL;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("Init failed: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(XRES, YRES, 32, SDL_OPENGL | SDL_HWSURFACE);
	if(!screen) {
		printf("Video mode init failed: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption("kgtd", "kgtd");
	atexit(SDL_Quit);

	/* set up GL
	 * (0,0) is upper left
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.0, 1.0, 0.0);
	glScalef(2.0/XRES, -2.0/YRES, 1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);

	/* initialize data structures */
	controls_init();
	graphics_init();
	text_init();
	reset();

	/* timer for updates */
	oldtime = SDL_GetTicks();
	if(SDL_AddTimer(25, timer_cb, NULL) == NULL) {
		printf("Error setting update timer...\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	SDL_Event ev;

	init();

	while (SDL_WaitEvent(&ev)) {
		do {
			handle_event(&ev);
		} while (SDL_PollEvent(&ev));
		if(need_draw) {
			need_draw = 0;
			draw();
		}
	}

	return 1;
}
