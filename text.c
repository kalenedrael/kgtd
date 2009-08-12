#include "globals.h"
#include "text.h"

/* for font bitmaps */
#include "font_data.c"

void text_init()
{
	int i;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	for(i = 0; i < 128; i++) {
		glNewList(DISPLAY_LIST_FONT_BASE + i, GL_COMPILE);
		glBitmap(FONT_WIDTH, FONT_HEIGHT, 0, 0, FONT_WIDTH, 0,
		         font_data[i]);
		glEndList();
	}
}

void text_draw(char *str, int x, int y)
{
	glRasterPos2i(x, y);
	while(*str > 0)
		glCallList(DISPLAY_LIST_FONT_BASE + *str++);
}
