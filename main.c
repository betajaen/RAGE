#include "data.h"
#include "functions.h"
#include "retro.c"

Font   FONT_NEOSANS;
Bitmap SPRITESHEET;
u32    COUNTER_FRAME;
u32    COUNTER_SECOND;
u16    PLAYER;

typedef enum
{
  CTRL_QUIT,
  CTRL_MOVE_LEFT,
  CTRL_MOVE_RIGHT,
  CTRL_MOVE_UP,
  CTRL_MOVE_DOWN,
  CTRL_MOVE_JUMP,
  CTRL_CROUCH,
  CTRL_HIT,
  CTRL_GUARD,
} Control;

void Init(Settings* settings)
{
  Palette_Make(&settings->palette);

  Palette_LoadFromBitmap("tile.png", &settings->palette);
  Bitmap_Load24("tile.png", &SPRITESHEET, 0xFF, 0x00, 0xFF);

  Font_Load("NeoSans.png", &FONT_NEOSANS, Colour_Make(0,0,255), Colour_Make(255,0,255));

  Input_BindKey(SDL_SCANCODE_ESCAPE, CTRL_QUIT);
  Input_BindKey(SDL_SCANCODE_W,      CTRL_MOVE_UP);
  Input_BindKey(SDL_SCANCODE_S,      CTRL_MOVE_DOWN);
  Input_BindKey(SDL_SCANCODE_A,      CTRL_MOVE_LEFT);
  Input_BindKey(SDL_SCANCODE_D,      CTRL_MOVE_RIGHT);
  Input_BindKey(SDL_SCANCODE_SPACE,  CTRL_MOVE_JUMP);
  Input_BindKey(SDL_SCANCODE_H,      CTRL_CROUCH);
  Input_BindKey(SDL_SCANCODE_J,      CTRL_HIT);
  Input_BindKey(SDL_SCANCODE_K,      CTRL_GUARD);

}

void Start()
{
  Objects_Setup();
  
  PLAYER = Objects_Create(OT_Player);
}

void Step()
{
  COUNTER_FRAME++;

  if (COUNTER_SECOND == 30)
  {
    COUNTER_FRAME = 0;
    COUNTER_SECOND++;
  }

  int nextMovement = 0;

  if (Input_GetActionDown(CTRL_MOVE_LEFT))
    nextMovement = MV_Left;
  else if (Input_GetActionDown(CTRL_MOVE_RIGHT))
    nextMovement = MV_Right;
  else if (Input_GetActionDown(CTRL_MOVE_UP))
    nextMovement = MV_Up;
  else if (Input_GetActionDown(CTRL_MOVE_DOWN))
    nextMovement = MV_Down;

  Objects_SetMovementVector(PLAYER, nextMovement);
  Objects_Tick();

  Objects_Draw();
}
