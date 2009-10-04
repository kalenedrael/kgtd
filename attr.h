/* @file attr.h
 * @brief attribute declarations
 *
 * This file may be eliminated depending on various architectural changes.
 */

#ifndef _ATTR_H_
#define _ATTR_H_

#include "globals.h"

#define ATTR_NUM 11

typedef enum attr {
	/* energy -> particle types */
	ATTR_PLASMA = 0,
	ATTR_LTNG = 1,

	/* energy -> laser types */
	ATTR_CW = 2,
	ATTR_PULSE = 3,

	/* mass -> kinetic */
	ATTR_APCR = 4,
	ATTR_APFSDS = 5,
	ATTR_DU = 6,

	/* mass -> explosive */
	ATTR_HE = 7,
	ATTR_HEAT = 8,
	ATTR_HESH = 9,

	ATTR_BASIC = 10,
	ATTR_NONE = 11
} attr_t;

extern float attr_colors[ATTR_NUM][3];
extern char *attr_dscr[ATTR_NUM];

#endif
