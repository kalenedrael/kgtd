#ifndef _ATTR_H_
#define _ATTR_H_

#include "globals.h"

#define ATTR_NUM 11

typedef enum __attribute__((__packed__)) attr {
	/* mass base type */
	ATTR_MASS_KINETIC,

	/* energy -> particle types */
	ATTR_ENERGY_PARTICLE_PLASMA,
	ATTR_ENERGY_PARTICLE_LIGHTNING,

	/* energy -> laser types */
	ATTR_ENERGY_LASER_PULSE,
	ATTR_ENERGY_LASER_CW,

	/* mass -> kinetic */
	ATTR_MASS_KINETIC_APCR,
	ATTR_MASS_KINETIC_APFSDS,
	ATTR_MASS_KINETIC_DU,

	/* mass -> explosive */
	ATTR_MASS_EXPLOSIVE_HE,
	ATTR_MASS_EXPLOSIVE_HEAT,
	ATTR_MASS_EXPLOSIVE_HESH

} attr_t;

extern float attr_colors[ATTR_NUM][3];

#endif
