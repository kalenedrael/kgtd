#ifndef _ATTR_H_
#define _ATTR_H_

#include "globals.h"

#define ATTR_NUM 11

typedef enum attr {
	/* mass base type */
	ATTR_MASS_KINETIC = 0,

	/* energy -> particle types */
	ATTR_ENERGY_PARTICLE_PLASMA = 1,
	ATTR_ENERGY_PARTICLE_LIGHTNING = 2,

	/* energy -> laser types */
	ATTR_ENERGY_LASER_PULSE = 3,
	ATTR_ENERGY_LASER_CW = 4,

	/* mass -> kinetic */
	ATTR_MASS_KINETIC_APCR = 5,
	ATTR_MASS_KINETIC_APFSDS = 6,
	ATTR_MASS_KINETIC_DU = 7,

	/* mass -> explosive */
	ATTR_MASS_EXPLOSIVE_HE = 8,
	ATTR_MASS_EXPLOSIVE_HEAT = 9,
	ATTR_MASS_EXPLOSIVE_HESH = 10,

	ATTR_NONE = 11
} attr_t;

extern float attr_colors[ATTR_NUM][3];

#endif
