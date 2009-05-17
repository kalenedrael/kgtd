#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include <math.h>

/* global constants */
#define XRES 800
#define YRES 600
#define SIDEBAR_SIZE 100
#define GRID_SIZE 20
#define GRID_X ((XRES-SIDEBAR_SIZE)/GRID_SIZE)
#define GRID_Y (YRES/GRID_SIZE)

#define TOWER_DEFAULT_MAX_ENERGY 1000.0

/* bullet constants */
#define BULLET_NUM_MAX 2048
#define BULLET_SIZE 3
#define BULLET_INVALID (-1)
#define BULLET_MAX_AGE 1.0
#define BULLET_PULSE_MAX_AGE 0.1
#define BULLET_DEFAULT_SPEED 500.1

/* XXX */
#define BULLET_MAX_RANGE 40000.0

/* noob constants */
#define NOOBS_NUM_MAX 1024
#define NOOB_SIZE 10
#define NOOB_DEFAULT_HP 9999.0
#define NOOB_INVALID (-1)
#define NOOB_SPEED_MULT 2.0

/* some forward declarations */
typedef struct noob_t noob_t;
typedef struct bullet_t bullet_t;
typedef struct tower_t tower_t;
typedef struct path_t path_t;
typedef struct state_t state_t;
typedef struct map_t map_t;
typedef struct level_t level_t;

#endif

