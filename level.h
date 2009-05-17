#ifndef _LEVEL_H_
#define _LEVEL_H_

struct map_t {
	int *path;
	int length;
	int x_start;
	int y_start;
};

struct level_t {
	int num;
};

#endif

