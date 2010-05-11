#include "damage.h"

extern Q_HEAD(noob_t) noob_list;

/* @brief damage table
 * all are fractions out of 128; 128 means no penalty or bonus
 * this may be need to be refactored
 */
static int armor_table[][3] = {
	/* base, composite, reflective */
	[ATTR_PLASMA] = {128, 128, 140},
	[ATTR_LTNG]   = {128, 128, 168},
	[ATTR_CW]     = {1920,  72,  64},
	[ATTR_PULSE]  = {1280, 128, 128},
	[ATTR_APCR]   = {128,  96, 140},
	[ATTR_APFSDS] = {128, 128, 168},
	[ATTR_DU]     = {128, 168, 192},
	[ATTR_HE]     = {128,  72, 128},
	[ATTR_HEAT]   = {128,  96, 128},
	[ATTR_HESH]   = {128, 128, 128},
	[ATTR_BASIC]  = {128,  96, 140}
};

static int shield_table[] = {
	[ATTR_PLASMA] = 256,
	[ATTR_LTNG]   = 128,
	[ATTR_CW]     = 2560,
	[ATTR_PULSE]  = 1920,
	[ATTR_APCR]   = 72,
	[ATTR_APFSDS] = 48,
	[ATTR_DU]     = 32,
	[ATTR_HE]     = 96,
	[ATTR_HEAT]   = 72,
	[ATTR_HESH]   = 144,
	[ATTR_BASIC]  = 48,
};

/* @brief splash range table */
static float splash_range[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1600.0, 400.0, 900.0};

/* calculates damage after armor bonuses/penalties */
static inline int dmg_normalize(noob_t *noob, int damage, attr_t attr)
{
	int i;

	damage = (damage * armor_table[attr][0]) / 128;
	for(i = 0; i < 2; i++) {
		if(noob->armor_type & (1 << i))
			damage = (damage * armor_table[attr][i+1]) / 128;
	}
	return damage;
}

static void do_damage(noob_t *noob, int damage, attr_t attr)
{
	int shield_dmg, shield = noob->shield;

	if(shield > 0) {
		shield_dmg = damage * shield_table[attr] / 128;

		if(shield_dmg >= shield) {
			damage = (shield_dmg - shield) * damage / shield_dmg;
			noob->shield = 0;
		}
		else {
			noob->shield -= shield_dmg;
			return;
		}
	}
	noob->hp -= dmg_normalize(noob, damage, attr);
}

void damage_calc(noob_t *noob, int damage, int dt, attr_t attr)
{
	noob_t *cur;
	float splash;

	if(attr == ATTR_NONE) {
		printf("ATTR_NONE: fail?\n");
		return;
	}

	if(attr == ATTR_LTNG)
		noob->stun_time = 800 /* ms */;

	damage = attr <= ATTR_PULSE ? damage * dt / 16384 : damage / 16;
	do_damage(noob, damage, attr);

	/* do splash dmg */
	splash = splash_range[attr];
	if(splash == 0.0)
		return;

	Q_FOREACH(cur, &noob_list, list) {
		if(noob == cur)
			continue;
		float d = distance2(&cur->pos, &noob->pos);
		if(d < splash)
			do_damage(cur, damage * (int)(splash - d) / (int)splash / 2, attr);
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
