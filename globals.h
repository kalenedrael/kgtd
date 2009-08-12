#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <math.h>

/* global constants */
#define XRES 800
#define YRES 608
#define GRID_SIZE 32
#define TOWER_SIZE 26
#define GRID_X (XRES/GRID_SIZE)
#define GRID_Y (YRES/GRID_SIZE)

#define TOWER_DEFAULT_MAX_ENERGY 100000

/* bullet constants */
#define BULLET_NUM_MAX 2048
#define BULLET_SIZE 3
#define BULLET_MAX_AGE 1000
#define BULLET_PULSE_MAX_AGE 100
#define BULLET_SPEED 320.1

/* XXX */
#define BULLET_MAX_RANGE 22500.0

/* noob constants */
#define NOOBS_NUM_MAX 1024
#define NOOB_DEFAULT_HP 932256
#define NOOB_DEFAULT_SPEED 35.0
#define NOOB_SIZE 15

/* some forward declarations */
typedef struct noob_t noob_t;
typedef struct bullet_t bullet_t;
typedef struct tower_t tower_t;
typedef struct path_t path_t;
typedef struct state_t state_t;
typedef struct map_t map_t;
typedef struct wave_t wave_t;
typedef struct level_t level_t;

enum {
	/* various objects */
	DISPLAY_LIST_TOWER = 1,
	DISPLAY_LIST_NOOB = 2,
	DISPLAY_LIST_PATH = 3,

	/* controls */
	DISPLAY_LIST_GRID = 4,
	DISPLAY_LIST_OCCLUDE = 5,
	DISPLAY_LIST_CIRCLE = 6,
	DISPLAY_LIST_X = 7,

	/* numerals */
	DISPLAY_LIST_NUM_BASE = 10,

	/* font */
	DISPLAY_LIST_FONT_BASE = 20
};

#endif

