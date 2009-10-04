/* @file attr.c
 * @brief tower/bullet colors by attribute
 *
 * This will probably need to be eliminated soon.
 */
 
#include "attr.h"

float attr_colors[ATTR_NUM][3] = {
	[ATTR_PLASMA] = {0.5, 0.5, 1.0},
	[ATTR_LTNG]   = {0.5, 0.5, 1.0},
	[ATTR_CW]     = {1.0, 0.0, 0.0},
	[ATTR_PULSE]  = {1.0, 0.0, 0.0},
	[ATTR_APCR]   = {0.4, 0.8, 0.4},
	[ATTR_APFSDS] = {0.4, 0.8, 0.4},
	[ATTR_DU]     = {0.4, 0.8, 0.4},
	[ATTR_HE]     = {1.0, 0.5, 0.0},
	[ATTR_HEAT]   = {1.0, 0.5, 0.0},
	[ATTR_HESH]   = {1.0, 0.5, 0.0},
	[ATTR_BASIC]  = {0.6, 0.6, 0.6}
};

char *attr_dscr[ATTR_NUM] = {
	[ATTR_PLASMA] = "plasma",
	[ATTR_LTNG]   = "lightning",
	[ATTR_CW]     = "continuous laser",
	[ATTR_PULSE]  = "pulsed laser",
	[ATTR_APCR]   = "AP gun",
	[ATTR_APFSDS] = "APFSDS gun",
	[ATTR_DU]     = "APFSDS-DU gun",
	[ATTR_HE]     = "high explosive gun",
	[ATTR_HEAT]   = "HEAT projectile gun",
	[ATTR_HESH]   = "HESH projectile gun",
	[ATTR_BASIC]  = "basic gun"
};
