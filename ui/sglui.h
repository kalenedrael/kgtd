#ifndef SGLUI_H
#define SGLUI_H

#include <SDL/SDL.h>
#include "../variable_queue.h"

enum {
	SGLUI_NONBLOCK = 0,
	SGLUI_BLOCK_CHILDREN = 1,
	SGLUI_BLOCK_ALL = 2
};

enum {
	SGLUI_MOVE = 0,
	SGLUI_ON = 1,
	SGLUI_OFF = 2
};

enum {
	SGLUI_NOIXN_IGNORE = -1,
	SGLUI_NOIXN = 0,
	SGLUI_IXN = 1,
	SGLUI_IXN_IGNORE = 2
};

struct sglui_obj;

/* function typedefs */
typedef int (*sglui_click_fn) (struct sglui_obj*, SDL_MouseButtonEvent *ev);
typedef int (*sglui_move_fn) (struct sglui_obj*, SDL_MouseMotionEvent *ev, int);
typedef int (*sglui_draw_fn) (struct sglui_obj*);
typedef int (*sglui_ixn_fn) (struct sglui_obj*, int, int);

/* a sglui object */
typedef struct sglui_obj {
	sglui_draw_fn draw;
	sglui_ixn_fn is_within;
	sglui_click_fn click;
	sglui_move_fn move;

	/* tree node information */
	struct sglui_obj *parent;
	Q_HEAD(sglui_obj) children;
	Q_NEW_LINK(sglui_obj) list;
} sglui_obj;

int sglui_ixn_always(sglui_obj *obj, int x, int y);
int sglui_ixn_never(sglui_obj *obj, int x, int y);

int sglui_init(sglui_ixn_fn head_fn);

/* object manipulation */
int sglui_obj_init(sglui_obj *obj, sglui_obj *parent,
                   sglui_draw_fn draw_cb, sglui_ixn_fn ixn_cb,
                   sglui_click_fn click_cb, sglui_move_fn move_cb);
void sglui_link(sglui_obj *obj, sglui_obj *parent);
void sglui_unlink(sglui_obj *child);

void sglui_handle_ev(SDL_Event *ev);
void sglui_draw_all(void);

#endif /* SGLUI_H */
