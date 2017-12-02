#include "data.h"
#include "functions.h"
#include "retro.c"

Font   FONT_NEOSANS;
Bitmap SPRITESHEET;

void Init(Settings* settings)
{
  Palette_Make(&settings->palette);

  Palette_LoadFromBitmap("tile.png", &settings->palette);
  Bitmap_Load24("tile.png", &SPRITESHEET, 0xFF, 0x00, 0xFF);

  Font_Load("NeoSans.png", &FONT_NEOSANS, Colour_Make(0,0,255), Colour_Make(255,0,255));
}

void Start()
{
}

void Step()
{
  Tile_Draw2(&SPRITESHEET, 10, 10,  0, 0, 16);
}
