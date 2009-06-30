#ifndef _UTIL_H_
#define _UTIL_H_

typedef struct {
	float x;
	float y;
} pos_t;

static inline float distance2(pos_t *pos1, pos_t *pos2)
{
	float dx = pos1->x - pos2->x, dy = pos1->y - pos2->y;
	return dx * dx + dy * dy;
}

#endif
