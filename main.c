#include "data.h"
#include "functions.h"
#include "retro.c"

Font   FONT_NEOSANS;
Bitmap SPRITESHEET;
Bitmap ANIMATIONS[2];
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

Palette CharacterSrcPalette;
Palette PlayerPalette;
Palette EnemyPalette;

inline Colour Make_RGB(u8 r, u8 g, u8 b)
{
  Colour c = { r, g, b};
  return c;
}

void Init(Settings* settings)
{
  Palette_Make(&settings->palette);

  Palette_LoadFromBitmap("tile.png", &settings->palette);
  Bitmap_Load24("tile.png", &SPRITESHEET, 0xFF, 0x00, 0xFF);


  CharacterSrcPalette.count = 4;
  CharacterSrcPalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  CharacterSrcPalette.colours[1] = Make_RGB(0x6c, 0x74, 0x7b);
  CharacterSrcPalette.colours[2] = Make_RGB(0xdc, 0xde, 0xe3);
  CharacterSrcPalette.colours[3] = Make_RGB(0xf0, 0xf8, 0xf7);

  PlayerPalette.count = 4;
  PlayerPalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  PlayerPalette.colours[1] = Make_RGB(0xa3, 0x37, 0x39);
  PlayerPalette.colours[2] = Make_RGB(0xb8, 0x3e, 0x40);
  PlayerPalette.colours[3] = Make_RGB(0xd0, 0x46, 0x48);

  EnemyPalette.count = 4;
  EnemyPalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  EnemyPalette.colours[1] = Make_RGB(0x6c, 0x74, 0x7b);
  EnemyPalette.colours[2] = Make_RGB(0xdc, 0xde, 0xe3);
  EnemyPalette.colours[3] = Make_RGB(0xf0, 0xf8, 0xf7);

  Bitmap_Load24_PaletteSwap("character.png", &ANIMATIONS[0], 0xFF, 0x00, 0xFF, &CharacterSrcPalette, &PlayerPalette);
  Bitmap_Load24_PaletteSwap("character.png", &ANIMATIONS[1], 0xFF, 0x00, 0xFF, &CharacterSrcPalette, &EnemyPalette);

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

  for(int i=0;i < 1;i++)
  {
    u16 enemy = Objects_Create(OT_Enemy);
    Objects_SetPosition(enemy, 300 + rand() % 100, rand() % 16);
    Objects_SetTrackingObject(enemy, PLAYER);
  }

  Canvas_SetFlags(0, CNF_Clear | CNF_Render, 4);
}

void Step()
{
  COUNTER_FRAME++;

  if (COUNTER_SECOND == 30)
  {
    COUNTER_FRAME = 0;
    COUNTER_SECOND++;
  }

  Objects_PreTick();

  int nextMovement = 0;

  if (Input_GetActionDown(CTRL_MOVE_LEFT))
    nextMovement = MV_Left;
  else if (Input_GetActionDown(CTRL_MOVE_RIGHT))
    nextMovement = MV_Right;
  else if (Input_GetActionDown(CTRL_MOVE_UP))
    nextMovement = MV_Up;
  else if (Input_GetActionDown(CTRL_MOVE_DOWN))
    nextMovement = MV_Down;

  int nextAction = 0;

  //if (Input_GetActionReleased(CTRL_HIT))
  //  nextAction = MA_Hit;
  //else 
  if (Input_GetActionDown(CTRL_CROUCH))
    nextAction = MA_Crouch;
  //else if (Input_GetActionReleased(CTRL_GUARD))
  //  nextAction = MA_Guard;

  Rect rect;
  rect.left   = 0;
  rect.right  = SCREEN_WIDTH;
  rect.top    = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE;
  rect.bottom = SCREEN_HEIGHT;
  Canvas_DrawFilledRectangle(5, rect);

  rect.left = 0;
  rect.right = SCREEN_WIDTH;
  rect.top = 0;
  rect.bottom = SCREEN_TOP_EDGE;
  Canvas_DrawFilledRectangle(5, rect);

  if (nextMovement != 0)
  {
    Objects_SetMovementVector(PLAYER, nextMovement);
  }

  if (nextAction != 0)
  {
    Objects_SetMovementAction(PLAYER, nextAction);
  }
  
  Objects_Tick();

  Objects_Draw();
}
