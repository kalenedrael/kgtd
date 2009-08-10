#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "globals.h"

#define SEL_SIZE 160
#define SEL_X SEL_SIZE
#define SEL_Y (YRES - SEL_SIZE)
#define SEL_BUFFER 35
#define BTN_SIZE (SEL_SIZE / 4)
#define BTN_OFFSET (BTN_SIZE - TOWER_SIZE)

void controls_init(void);
void controls_draw(int x, int y, state_t *state);
void controls_click(SDL_MouseButtonEvent *ev, state_t *state);

#endif

