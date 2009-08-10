#include "damage.h"

/* @brief damage table
 * all are fractions out of 128; 128 means no penalty or bonus
 * this may be need to be refactored
 */
static int armor_dmg[][5] = {
	/* base, composite, reactive, reflective, regen */
	{128,  96, 128, 140,   0}, /* ENERGY_PARTICLE_PLASMA */
	{128, 128, 168, 168, -32}, /* ENERGY_PARTICLE_LIGHTNING */
	{128,  72,  72,  32, -32}, /* ENERGY_LASER_CW */
	{12800, 128, 128, 128, 128}, /* ENERGY_LASER_PULSE */
	{128,  96, 128, 140, 128}, /* MASS_KINETIC_APCR */
	{128, 128, 168, 168, 192}, /* MASS_KINETIC_APFSDS */
	{128, 168, 180, 192, 192}, /* MASS_KINETIC_DU */
	{128,  72,  72, 128, 128}, /* MASS_EXPLOSIVE_HE */
	{128,  96, 128, 128, 128}, /* MASS_EXPLOSIVE_HEAT */
	{128, 128,  96, 128, 128}, /* MASS_EXPLOSIVE_HESH */
	{128,  96, 128, 140,   0}, /* BASIC */
};

/* calculates damage after armor bonuses/penalties */
static inline int dmg_normalize_armor(noob_t *noob, int damage, attr_t attr)
{
	int i;

	damage = (damage * armor_dmg[attr][0]) / 128;
	for(i = 0; i < 4; i++) {
		if(noob->armor_type & (1 << i))
			damage = (damage * armor_dmg[attr][i+1]) / 128;
	}
	return damage;
}

/* XXX no shield damage yet */
void damage_calc(noob_t *noob, int damage, int dt, attr_t attr)
{
	if(attr == ATTR_NONE) {
		printf("ATTR_NONE: fail?\n");
		return;
	}

	if(attr == ATTR_ENERGY_PARTICLE_LIGHTNING)
		noob->stun_time = 50;

	damage = dmg_normalize_armor(noob, damage, attr);
	if(attr <= ATTR_ENERGY_LASER_PULSE)
		/* sort of like 1000, except power of two for speed -.- */
		noob->hp -= (damage * dt) / 1024;
	else
		noob->hp -= damage;
}

/* @brief calculates if a target is worth firing at */
int damage_not_worthwhile(noob_t *noob, attr_t attr)
{
	if(noob == NULL || noob->hp <= 0 || noob->is_dead ||
	   ((noob->future_stun || noob->stun_time) &&
	    attr == ATTR_ENERGY_PARTICLE_LIGHTNING))
		return 1;

	return 0;
}
