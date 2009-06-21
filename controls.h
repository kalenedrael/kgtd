#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "globals.h"

void controls_init(void);
void controls_draw(int x, int y, state_t *state);
void controls_click(int x, int y, state_t *state);

#endif

