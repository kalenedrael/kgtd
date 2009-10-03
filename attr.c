/* @file attr.c
 * @brief tower/bullet colors by attribute
 *
 * This will probably need to be eliminated soon.
 */
 
#include "attr.h"

float attr_colors[ATTR_NUM][3] = {
	{0.5, 0.5, 1.0}, /* ATTR_ENERGY_PARTICLE_PLASMA */
	{0.5, 0.5, 1.0}, /* ATTR_ENERGY_PARTICLE_LIGHTNING */
	{1.0, 0.0, 0.0}, /* ATTR_ENERGY_LASER_CW */
	{1.0, 0.0, 0.0}, /* ATTR_ENERGY_LASER_PULSE */
	{0.4, 0.8, 0.4}, /* ATTR_MASS_KINETIC_APCR */
	{0.4, 0.8, 0.4}, /* ATTR_MASS_KINETIC_APFSDS */
	{0.4, 0.8, 0.4}, /* ATTR_MASS_KINETIC_DU */
	{1.0, 0.5, 0.0}, /* ATTR_MASS_EXPLOSIVE_HE */
	{1.0, 0.5, 0.0}, /* ATTR_MASS_EXPLOSIVE_HEAT */
	{1.0, 0.5, 0.0}, /* ATTR_MASS_EXPLOSIVE_HESH */
	{0.6, 0.6, 0.6}  /* ATTR_BASIC */
};

char *attr_dscr[ATTR_NUM] = {
	"plasma", /* ATTR_ENERGY_PARTICLE_PLASMA */
	"lightning", /* ATTR_ENERGY_PARTICLE_LIGHTNING */
	"continuous laser", /* ATTR_ENERGY_LASER_CW */
	"pulsed laser", /* ATTR_ENERGY_LASER_PULSE */
	"AP gun", /* ATTR_MASS_KINETIC_APCR */
	"APFSDS gun", /* ATTR_MASS_KINETIC_APFSDS */
	"APFSDS-DU gun", /* ATTR_MASS_KINETIC_DU */
	"high explosive gun", /* ATTR_MASS_EXPLOSIVE_HE */
	"HEAT projectile gun", /* ATTR_MASS_EXPLOSIVE_HEAT */
	"HESH projectile gun", /* ATTR_MASS_EXPLOSIVE_HESH */
	"basic gun"  /* ATTR_BASIC */
};
