#include "level.h"
#include "noob.h"

/* path format is distance in x, distance in y, distance in x, ...
 * no builtin intersection detection or out of bounds detection
 */
static int path1[] = {5, -3, 4, 4, -3, 6, 12, -10};
static int path2[] = {8, 5, 2, -10, 2, 5, 9};

static level_t level1, level2;
static wave_t wave11, wave12, wave13, wave14, wave15, wave16, wave17,
              wave18, wave19, wave21, wave22, wave23, wave24;

level_t *levels = &level1;

/***************\
 *** level 1 ***
\***************/

static level_t level1 = {
	.next = &level2,
	.map = { path1, sizeof(path1)/sizeof(*path1), 3, 6 },
	.waves = &wave11
};

static wave_t wave11 = {
	.next = &wave12,
	.noobs = 10,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP,
	.delay = 500,
	.shield = 0,
	.armor_type = 0,
};

static wave_t wave12 = {
	.next = &wave13,
	.noobs = 20,
	.speed = NOOB_DEFAULT_SPEED/2,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = 0,
	.armor_type = 0,
};

static wave_t wave13 = {
	.next = &wave14,
	.noobs = 20,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = 0,
	.armor_type = 0,
};

static wave_t wave14 = {
	.next = &wave15,
	.noobs = 30,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = 0,
};

static wave_t wave15 = {
	.next = &wave16,
	.noobs = 40,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = ARMOR_COMPOSITE,
};

static wave_t wave16 = {
	.next = &wave17,
	.noobs = 50,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = ARMOR_REFLECTIVE,
};

static wave_t wave17 = {
	.next = &wave18,
	.noobs = 60,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = ARMOR_COMPOSITE | ARMOR_REFLECTIVE,
};

static wave_t wave18 = {
	.next = &wave19,
	.noobs = 70,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = ARMOR_COMPOSITE,
};

static wave_t wave19 = {
	.next = NULL,
	.noobs = 1,
	.speed = NOOB_DEFAULT_SPEED/2,
	.hp = NOOB_DEFAULT_HP * 100,
	.delay = 0,
	.shield = NOOB_DEFAULT_HP * 100,
	.armor_type = ARMOR_COMPOSITE | ARMOR_REFLECTIVE,
};

/***************\
 *** level 2 ***
\***************/

static level_t level2 = {
	.next = NULL,
	.map = { path2, sizeof(path2)/sizeof(*path2), 2, 8 },
	.waves = &wave21
};

static wave_t wave21 = {
	.next = &wave22,
	.noobs = 10,
	.speed = NOOB_DEFAULT_SPEED/2,
	.hp = NOOB_DEFAULT_HP * 5,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = 0,
};

static wave_t wave22 = {
	.next = &wave23,
	.noobs = 20,
	.speed = NOOB_DEFAULT_SPEED * 2,
	.hp = NOOB_DEFAULT_HP * 2,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = 0,
};

static wave_t wave23 = {
	.next = &wave24,
	.noobs = 40,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 5,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = 0,
};

static wave_t wave24 = {
	.next = NULL,
	.noobs = 50,
	.speed = NOOB_DEFAULT_SPEED,
	.hp = NOOB_DEFAULT_HP * 6,
	.delay = 500,
	.shield = NOOB_DEFAULT_HP,
	.armor_type = ARMOR_COMPOSITE,
};
