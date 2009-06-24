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

#define EV_UPDATE 0
#define EV_SPAWN 1

/* function prototypes */
static void draw();
static void update();
static void reset();

/* SDL stuff */
static SDL_Surface *screen;

/* timekeeper */
static int oldtime;
/* mouse position */
static int mouse_x, mouse_y;

/* game state */
state_t kgtd_state;

static void handle_event(SDL_Event *ev)
{
	switch(ev->type) {
	case SDL_QUIT:
		exit(0);
	case SDL_KEYUP: {
		SDLKey sym = ev->key.keysym.sym;
		switch(sym) {
		case SDLK_0:
			kgtd_state.type_selected = ATTR_ENERGY_PARTICLE_PLASMA;
			break;
		case SDLK_1:
			kgtd_state.type_selected = ATTR_ENERGY_PARTICLE_LIGHTNING;
			break;
		case SDLK_2:
			kgtd_state.type_selected = ATTR_ENERGY_LASER_PULSE;
			break;
		case SDLK_3:
			kgtd_state.type_selected = ATTR_ENERGY_LASER_CW;
			break;
		case SDLK_4:
			kgtd_state.type_selected = ATTR_MASS_KINETIC_APCR;
			break;
		case SDLK_5:
			kgtd_state.type_selected = ATTR_MASS_KINETIC_APFSDS;
			break;
		case SDLK_6:
			kgtd_state.type_selected = ATTR_MASS_KINETIC_DU;
			break;
		case SDLK_7:
			kgtd_state.type_selected = ATTR_MASS_EXPLOSIVE_HE;
			break;
		case SDLK_8:
			kgtd_state.type_selected = ATTR_MASS_EXPLOSIVE_HEAT;
			break;
		case SDLK_9:
			kgtd_state.type_selected = ATTR_MASS_EXPLOSIVE_HESH;
			break;
		case SDLK_EQUALS:
			kgtd_state.type_selected = ATTR_NONE;
			break;
		case SDLK_r:
			reset();
			break;
		default: break;
		}
		return;
	}
	case SDL_MOUSEBUTTONUP: {
		int x = ev->button.x;
		int y = ev->button.y;
		int gx = x/GRID_SIZE;
		int gy = y/GRID_SIZE;

		/* XXX */
		if(ev->button.button == SDL_BUTTON_LEFT &&
		   kgtd_state.type_selected != ATTR_NONE)
			tower_new(gx, gy, 252, kgtd_state.type_selected);
		return;
	}
	case SDL_MOUSEMOTION:
		mouse_x = ev->motion.x;
		mouse_y = ev->motion.y;
		return;
	case SDL_USEREVENT: {
		if(ev->user.code == EV_UPDATE) {
			draw();
			update();
		}
		else if(ev->user.code == EV_SPAWN) {
			level_spawn(&kgtd_state);
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
	tev.user.code = EV_UPDATE;
	SDL_PushEvent(&tev);
	return x;
}

static Uint32 spawn_cb(Uint32 x, void* p)
{
	SDL_Event tev;
	tev.user.type = SDL_USEREVENT;
	tev.user.code = EV_SPAWN;
	SDL_PushEvent(&tev);
	return x;
}

static void update(void)
{
	int newtime = SDL_GetTicks();
	int idt = newtime - oldtime;
	float dt = (float)idt / 1000.0;

	noob_update_all(dt, &kgtd_state);
	bullet_update_all(dt, idt);
	tower_update_all(dt, idt);
	level_update(&kgtd_state);

	oldtime = newtime;
}

static void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
	        GL_STENCIL_BUFFER_BIT);

	controls_draw(mouse_x, mouse_y, &kgtd_state);
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
	reset();

	/* timer for updates */
	oldtime = SDL_GetTicks();
	if(SDL_AddTimer(25, timer_cb, NULL) == NULL) {
		printf("Error setting update timer...\n");
		exit(1);
	}
	if(SDL_AddTimer(150, spawn_cb, NULL) == NULL) {
		printf("Error setting spawn timer...\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	SDL_Event ev;

	init();

	while (SDL_WaitEvent(&ev))
		handle_event(&ev);

	return 1;
}

