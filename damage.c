#include "damage.h"

void damage_calc(noob_t *noob, float damage, float dt, attr_t attr)
{
	switch(attr) {
	case ATTR_MASS_KINETIC:
	case ATTR_MASS_KINETIC_APCR:
	case ATTR_MASS_KINETIC_APFSDS:
	case ATTR_MASS_KINETIC_DU:
	case ATTR_MASS_EXPLOSIVE_HE:
	case ATTR_MASS_EXPLOSIVE_HEAT:
		noob->hp -= damage;
		break;
	case ATTR_MASS_EXPLOSIVE_HESH:
	case ATTR_ENERGY_LASER_PULSE:
		noob->hp -= damage;
		break;
	case ATTR_ENERGY_PARTICLE_LIGHTNING:
		noob->stun_time = 50;
	case ATTR_ENERGY_PARTICLE_PLASMA:
	case ATTR_ENERGY_LASER_CW:
		noob->hp -= damage * dt;
	}

}

void damage_future_calc(noob_t *noob, float damage, attr_t attr)
{
	switch(attr) {
	case ATTR_ENERGY_LASER_PULSE:
		noob->future_hp -= damage * BULLET_PULSE_MAX_AGE;
		break;
	case ATTR_MASS_KINETIC:
	case ATTR_MASS_KINETIC_APCR:
	case ATTR_MASS_KINETIC_APFSDS:
	case ATTR_MASS_KINETIC_DU:
	case ATTR_MASS_EXPLOSIVE_HE:
	case ATTR_MASS_EXPLOSIVE_HEAT:
		noob->future_hp -= damage;
		break;
	case ATTR_ENERGY_PARTICLE_LIGHTNING:
		noob->future_stun = 1;
		noob->future_hp -= damage * (BULLET_PULSE_MAX_AGE / BULLET_MAX_AGE);
		break;
	case ATTR_MASS_EXPLOSIVE_HESH:
	case ATTR_ENERGY_PARTICLE_PLASMA:
	case ATTR_ENERGY_LASER_CW:
		noob->future_hp -= damage;
		break;
	}	
}

int damage_not_worthwhile(noob_t *noob, attr_t attr)
{
	if(noob == NULL || noob->future_hp <= 0.0 || noob->is_dead ||
	   (noob->future_stun && attr == ATTR_ENERGY_PARTICLE_LIGHTNING))
		return 1;

	return 0;
}

