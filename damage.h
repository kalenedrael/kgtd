#ifndef _DAMAGE_H_
#define _DAMAGE_H_

#include "attr.h"
#include "noob.h"

void damage_calc(noob_t *noob, int damage, int dt, attr_t attr);
int damage_not_worthwhile(noob_t *noob, attr_t attr);

#endif
