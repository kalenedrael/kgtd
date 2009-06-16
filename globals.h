#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include <math.h>

/* global constants */
#define XRES 800
#define YRES 600
#define SIDEBAR_SIZE 110
#define GRID_SIZE 30
#define GRID_X ((XRES-SIDEBAR_SIZE)/GRID_SIZE)
#define GRID_Y (YRES/GRID_SIZE)

#define TOWER_DEFAULT_MAX_ENERGY 100000

/* bullet constants */
#define BULLET_NUM_MAX 2048
#define BULLET_SIZE 3
#define BULLET_MAX_AGE 1000
#define BULLET_PULSE_MAX_AGE 100
#define BULLET_DEFAULT_SPEED 520.1

/* XXX */
#define BULLET_MAX_RANGE 40000.0

/* noob constants */
#define NOOBS_NUM_MAX 1024
#define NOOB_SIZE 10
#define NOOB_DEFAULT_HP 932256
#define NOOB_SPEED_MULT 50.0

/* some forward declarations */
typedef struct noob_t noob_t;
typedef struct bullet_t bullet_t;
typedef struct tower_t tower_t;
typedef struct path_t path_t;
typedef struct state_t state_t;
typedef struct map_t map_t;
typedef struct level_t level_t;

#endif

