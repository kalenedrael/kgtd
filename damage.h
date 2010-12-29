#ifndef _DAMAGE_H_
#define _DAMAGE_H_

#include "tower_types.h"
#include "noob.h"

void damage_calc(noob_t *noob, int damage, int dt, ttype_t type);
int damage_not_worthwhile(noob_t *noob, ttype_t type);

#endif
