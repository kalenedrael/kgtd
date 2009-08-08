#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "sglui.h"

typedef struct sglui_btn {
	sglui_obj obj;
	void (*btn_cb)(void);
	int x;
	int y;
	int w;
	int h;
} sglui_btn;

/* Yeah, you do *all* the drawing yourself. Suck it up. */
int sglui_button_init(sglui_btn *btn, sglui_obj *parent, int x, int y, int w,
                      int h, sglui_draw_fn draw, void(*btn_cb)(void));

#endif
