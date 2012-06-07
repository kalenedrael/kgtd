#include "damage.h"

extern Q_HEAD(noob_t) noob_list;

/* @brief damage table
 * all are fractions out of 128; 128 means no penalty or bonus
 * this may be need to be refactored
 */
static int armor_table[][3] = {
	/* base, composite, reflective */
	[TT_PLASMA] = {64,   128, 96},
	[TT_LTNG]   = {128,  128, 128},
	[TT_CW]     = {1920, 128, 32},
	[TT_PULSE]  = {1280, 128, 48},
	[TT_APCR]   = {128,  48,  128},
	[TT_APFSDS] = {144,  48,  128},
	[TT_DU]     = {192,  64,  128},
	[TT_HE]     = {72,   48,  128},
	[TT_HEAT]   = {128,  64,  128},
	[TT_AREA]   = {128,  96,  96},
	[TT_BASIC]  = {64,   64,  64}
};

static int shield_table[] = {
	[TT_PLASMA] = 512,
	[TT_LTNG]   = 128,
	[TT_CW]     = 1280,
	[TT_PULSE]  = 1920,
	[TT_APCR]   = 64,
	[TT_APFSDS] = 48,
	[TT_DU]     = 40,
	[TT_HE]     = 96,
	[TT_HEAT]   = 72,
	[TT_AREA]   = 128,
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
	[TT_HEAT]   = 900.0,
	[TT_AREA]   = 10000.0,
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

void damage_calc(bullet_t *bullet, int dt)
{
	noob_t *cur, *noob = bullet->dest;
	ttype_t type = bullet->type;
	int damage = bullet->damage;
	float splash;
	pos_t *pos;

	if(type == TT_NONE) {
		printf("ATTR_NONE: fail?\n");
		return;
	}

	if(type == TT_LTNG)
		noob->stun_time = 800 /* ms */;

	damage = type <= TT_PULSE || type == TT_AREA ? damage * dt / 16384 : damage / 16;
	if(type != TT_AREA) {
		do_damage(noob, damage, type);
		pos = &noob->pos;
	}
	else {
		pos = &bullet->pos;
		noob = NULL;
	}

	/* do splash dmg */
	splash = splash_range[type];
	if(splash == 0.0)
		return;

	Q_FOREACH(cur, &noob_list, list) {
		if(noob == cur)
			continue;
		float d = distance2(&cur->pos, pos);
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
	else
		return 0;
}
