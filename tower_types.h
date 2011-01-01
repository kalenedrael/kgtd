/* @file tower_types.h
 * @brief tower type declarations
 */

#ifndef _TT_H_
#define _TT_H_

#include "globals.h"

#define TT_NUM 11

typedef enum ttype {
	/* energy -> particle types */
	TT_PLASMA = 0,
	TT_LTNG = 1,

	/* energy -> laser types */
	TT_CW = 2,
	TT_PULSE = 3,

	/* mass -> kinetic */
	TT_APCR = 4,
	TT_APFSDS = 5,
	TT_DU = 6,

	/* mass -> explosive */
	TT_HE = 7,
	TT_HEAT = 8,
	TT_AREA = 9,

	TT_BASIC = 10,
	TT_NONE = 11
} ttype_t;

struct tt_data_t {
	float color[3];
	char *dscr;
	struct {
		void (*upd)(tower_t*, int);
		float range;
	} tower;
	struct {
		void (*update)(bullet_t*, float, int);
		void (*draw)(bullet_t*);
		unsigned int max_age;
	} bullet;
};

extern struct tt_data_t tt_data[TT_NUM];

#endif
