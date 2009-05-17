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

int path[] = {3, 7, 2, -7, 2, 5, 4, 6, 12};
map_t map = { path, sizeof(path)/sizeof(path[0]), 0, 3 };

/* function prototypes */
static void step();
static void update();
static void reset();

/* SDL stuff */
SDL_Surface *screen;
SDL_TimerID update_timer;
SDL_TimerID noobspawner;
SDL_TimerID noobspawner2;

/* timekeeper */
double oldtime;

/* game state */
state_t kgtd_state;

static void handle_event(SDL_Event *ev)
{
	switch(ev->type) {
	case SDL_QUIT:
		exit(0);
	case SDL_KEYUP: {
		if(ev->key.keysym.sym == SDLK_r)
			reset();
		return;
	}
	case SDL_MOUSEBUTTONUP: {
		int x = ev->button.x;
		int y = ev->button.y;
		if(x < SIDEBAR_SIZE) {
			controls_click(x, y, &kgtd_state);
			return;
		}
		int gx = (x - SIDEBAR_SIZE)/GRID_SIZE;
		int gy = y/GRID_SIZE;

		/* XXX */
		if(ev->button.button == SDL_BUTTON_LEFT)
			tower_new(gx, gy, 524.0, kgtd_state.type_selected);
		return;
	}
	case SDL_USEREVENT: {
		if(ev->user.code == 0) {
			step();
			update();
		}
		else if(ev->user.code == 1) {
			noob_new(GRID_SIZE/2, 3 * GRID_SIZE + GRID_SIZE/2, &kgtd_state);
		}
	}
	default:
		return;
	}
}

static Uint32 timer_cb(Uint32 x, void* p)
{
	SDL_Event tev;
	tev.user.type = SDL_USEREVENT;
	tev.user.code = 0;
	SDL_PushEvent(&tev);
	return x;
}

static Uint32 spawn_cb(Uint32 x, void* p)
{
	SDL_Event tev;
	tev.user.type = SDL_USEREVENT;
	tev.user.code = 1;
	SDL_PushEvent(&tev);
	return x;
}

static void update(void)
{
	double newtime = (double)(SDL_GetTicks())/1000.0;
	float dt = (float)(newtime - oldtime);

	noob_update_all(dt, &kgtd_state);
	bullet_update_all(dt);
	tower_update_all(dt);

	oldtime = newtime;
}

static void step(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslated(SIDEBAR_SIZE, 0, 0);
	path_draw_all(&kgtd_state);
	noob_draw_all();
	tower_draw_all();
	bullet_draw_all();
	glPopMatrix();
	controls_draw(&kgtd_state);

	SDL_GL_SwapBuffers();
}

static void reset(void)
{
	grid_init();
	bullet_init();
	noob_init();
	tower_init();
	state_reset(&kgtd_state);
	/* XXX */
	path_load(&kgtd_state, &map);
}

static void init(void)
{
	/* set up SDL */
	screen = NULL;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("Init failed: %s\n", SDL_GetError());
		exit(1);
	}
	screen = SDL_SetVideoMode(XRES, YRES, 32, SDL_OPENGL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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
	glTranslated(-1, 1, 0.0);
	glScaled(2.0/XRES, -2.0/YRES, 1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* initialize data structures */
	controls_init();
	reset();

	/* timer for updates */
	oldtime = ((double)clock())/(double)CLOCKS_PER_SEC;
	update_timer = SDL_AddTimer(25, timer_cb, NULL);
	if(update_timer == NULL) {
		printf("Error setting timer...\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	SDL_Event ev;

	init();

	/* XXX */
	noobspawner = SDL_AddTimer(3002, spawn_cb, NULL);

	while (SDL_WaitEvent(&ev)) {
		handle_event(&ev);
	}
	return 1;
}

