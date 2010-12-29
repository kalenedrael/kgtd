#include "damage.h"

extern Q_HEAD(noob_t) noob_list;

/* @brief damage table
 * all are fractions out of 128; 128 means no penalty or bonus
 * this may be need to be refactored
 */
static int armor_table[][3] = {
	/* base, composite, reflective */
	[TT_PLASMA] = {128, 128, 140},
	[TT_LTNG]   = {128, 128, 168},
	[TT_CW]     = {1920,  72,  64},
	[TT_PULSE]  = {1280, 128, 128},
	[TT_APCR]   = {128,  96, 140},
	[TT_APFSDS] = {128, 128, 168},
	[TT_DU]     = {128, 168, 192},
	[TT_HE]     = {128,  72, 128},
	[TT_HEAT]   = {128,  96, 128},
	[TT_AREA]   = {128, 128, 128},
	[TT_BASIC]  = {128,  96, 140}
};

static int shield_table[] = {
	[TT_PLASMA] = 256,
	[TT_LTNG]   = 128,
	[TT_CW]     = 2560,
	[TT_PULSE]  = 1920,
	[TT_APCR]   = 72,
	[TT_APFSDS] = 48,
	[TT_DU]     = 32,
	[TT_HE]     = 96,
	[TT_HEAT]   = 72,
	[TT_AREA]   = 144,
	[TT_BASIC]  = 48,
};

/* @brief splash range table */
static float splash_range[] = {
	[TT_PLASMA] = 0.0,
	[TT_LTNG]   = 0.0,
	[TT_CW]     = 0.0,
	[TT_PULSE]  = 0.0,
	[TT_APCR]   = 0.0,
	[TT_APFSDS] = 0.0,
	[TT_DU]     = 0.0,
	[TT_HE]     = 1600.0,
	[TT_HEAT]   = 400.0,
	[TT_AREA]   = 6400.0,
	[TT_BASIC]  = 0.0,
};

/* calculates damage after armor bonuses/penalties */
static inline int dmg_normalize(noob_t *noob, int damage, ttype_t type)
{
	int i;

	damage = (damage * armor_table[type][0]) / 128;
	for(i = 0; i < 2; i++) {
		if(noob->armor_type & (1 << i))
			damage = (damage * armor_table[type][i+1]) / 128;
	}
	return damage;
}

static void do_damage(noob_t *noob, int damage, ttype_t type)
{
	int shield_dmg, shield = noob->shield;

	if(shield > 0) {
		shield_dmg = damage * shield_table[type] / 128;

		if(shield_dmg >= shield) {
			damage = (shield_dmg - shield) * damage / shield_dmg;
			noob->shield = 0;
		}
		else {
			noob->shield -= shield_dmg;
			return;
		}
	}
	noob->hp -= dmg_normalize(noob, damage, type);
}

void damage_calc(noob_t *noob, int damage, int dt, ttype_t type)
{
	noob_t *cur;
	float splash;

	if(type == TT_NONE) {
		printf("ATTR_NONE: fail?\n");
		return;
	}

	if(type == TT_LTNG)
		noob->stun_time = 800 /* ms */;

	damage = type <= TT_PULSE ? damage * dt / 16384 : damage / 16;
	do_damage(noob, damage, type);

	/* do splash dmg */
	splash = splash_range[type];
	if(splash == 0.0)
		return;

	Q_FOREACH(cur, &noob_list, list) {
		if(noob == cur)
			continue;
		float d = distance2(&cur->pos, &noob->pos);
		if(d < splash)
			do_damage(cur, damage * (int)(splash - d) / (int)splash / 2, type);
	}
}

/* @brief calculates if a target is worth firing at */
int damage_not_worthwhile(noob_t *noob, ttype_t type)
{
	if(noob == NULL || noob->hp <= 0 || noob->is_dead ||
	   ((noob->future_stun || noob->stun_time > 0) && type == TT_LTNG))
		return 1;

	return 0;
}
