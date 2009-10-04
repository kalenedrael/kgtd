#include "damage.h"

extern Q_HEAD(noob_t) noob_list;

/* @brief damage table
 * all are fractions out of 128; 128 means no penalty or bonus
 * this may be need to be refactored
 */
static int armor_dmg[][5] = {
	/* base, composite, reactive, reflective, regen */
	[ATTR_PLASMA] = {128,  96, 128, 140,   0},
	[ATTR_LTNG]   = {128, 128, 168, 168, 32},
	[ATTR_CW]     = {1280,  72,  72,  32, 32},
	[ATTR_PULSE]  = {1280, 128, 128, 128, 128},
	[ATTR_APCR]   = {128,  96, 128, 140, 128},
	[ATTR_APFSDS] = {128, 128, 168, 168, 192},
	[ATTR_DU]     = {128, 168, 180, 192, 192},
	[ATTR_HE]     = {128,  72,  72, 128, 128},
	[ATTR_HEAT]   = {128,  96, 128, 128, 128},
	[ATTR_HESH]   = {128, 128,  96, 128, 128},
	[ATTR_BASIC]  = {128,  96, 128, 140,   0}
};

/* @brief splash range table */
static float splash_range[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1600.0, 400.0, 900.0};

/* calculates damage after armor bonuses/penalties */
static inline int dmg_normalize(noob_t *noob, int damage, attr_t attr)
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
	noob_t *cur;
	float splash;
	int norm_damage;

	if(attr == ATTR_NONE) {
		printf("ATTR_NONE: fail?\n");
		return;
	}

	if(attr == ATTR_LTNG)
		noob->stun_time = 800;

	norm_damage = dmg_normalize(noob, damage, attr);
	/* XXX HACK: all cw types must be less than ATTR_PULSE */
	if(attr <= ATTR_PULSE)
		/* sort of like 1000, except power of two for speed -.- */
		noob->hp -= (norm_damage * dt) / 1024;
	else
		noob->hp -= norm_damage;

	/* do splash damage */
	splash = splash_range[attr];
	if(splash == 0.0)
		return;

	Q_FOREACH(cur, &noob_list, list) {
		if(noob == cur)
			continue;
		float d = distance2(&cur->pos, &noob->pos);
		if(d < splash) {
			norm_damage = dmg_normalize(cur, damage, attr);
			cur->hp -= (norm_damage * (int)(splash - d)) / ((int)splash * 2);
		}
	}
}

/* @brief calculates if a target is worth firing at */
int damage_not_worthwhile(noob_t *noob, attr_t attr)
{
	if(noob == NULL || noob->hp <= 0 || noob->is_dead ||
	   ((noob->future_stun || noob->stun_time > 0) && attr == ATTR_LTNG))
		return 1;

	return 0;
}
