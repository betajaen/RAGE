#ifndef DATA_H
#define DATA_H

#define RETRO_WINDOW_CAPTION "RAGE"
#define RETRO_ARENA_SIZE Kilobytes(512)
#define RETRO_WINDOW_DEFAULT_WIDTH 1280
#define RETRO_WINDOW_DEFAULT_HEIGHT 720
#define RETRO_CANVAS_DEFAULT_WIDTH (RETRO_WINDOW_DEFAULT_WIDTH / 2)
#define RETRO_CANVAS_DEFAULT_HEIGHT (RETRO_WINDOW_DEFAULT_HEIGHT / 2)

#include "retro.h"

extern Font   FONT_NEOSANS;
extern Bitmap SPRITESHEET;

#endif