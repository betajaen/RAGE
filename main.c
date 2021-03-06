#include "data.h"
#include "functions.h"
#include "retro.c"

Font   FONT_KAGESANS;
Bitmap SPRITESHEET;
Bitmap ANIMATIONS[OT_COUNT];
Sound  HIT_SOUNDS[18];

u32    COUNTER_FRAME;
u32    COUNTER_SECOND;
u16    PLAYER;

Palette CharacterSrcPalette;
Palette PlayerPalette;
Palette EnemyPalette;
Palette CorpsePalette;

int levelState  = 0;
int levelTimer  = 0;
int levelOffset = 0;

void Title();
void Game();
void StartGame();
void Win();

u8 mode = 0;

inline Colour Make_RGB(u8 r, u8 g, u8 b)
{
  Colour c = { r, g, b};
  return c;
}

inline Rect Make_Rect(i32 x, i32 y, i32 w, i32 h)
{
  Rect r;
  r.left = x;
  r.top = y;
  r.right = x + w;
  r.bottom = y + h;
  return r;
}

inline Rect Make_Rect2(i32 l, i32 t, i32 r, i32 b)
{
  Rect rect;
  rect.left = l;
  rect.top = t;
  rect.right = r;
  rect.bottom = b;
  return rect;
}

void Init(Settings* settings)
{
  Sound_Load(&HIT_SOUNDS[0],  "Hit0.wav");
  Sound_Load(&HIT_SOUNDS[1],  "Hit1.wav");
  Sound_Load(&HIT_SOUNDS[2],  "Hit2.wav");
  Sound_Load(&HIT_SOUNDS[3],  "Hit3.wav");
  Sound_Load(&HIT_SOUNDS[4],  "Hit4.wav");
  Sound_Load(&HIT_SOUNDS[5],  "Hit5.wav");
  Sound_Load(&HIT_SOUNDS[6],  "Hit6.wav");
  Sound_Load(&HIT_SOUNDS[7],  "Hit7.wav");
  Sound_Load(&HIT_SOUNDS[8],  "Hit8.wav");
  Sound_Load(&HIT_SOUNDS[9],  "Hit9.wav");
  Sound_Load(&HIT_SOUNDS[10], "Hit10.wav");
  Sound_Load(&HIT_SOUNDS[11], "Hit11.wav");
  Sound_Load(&HIT_SOUNDS[12], "Hit12.wav");
  Sound_Load(&HIT_SOUNDS[13], "Hit13.wav");
  Sound_Load(&HIT_SOUNDS[14], "Hit14.wav");
  Sound_Load(&HIT_SOUNDS[15], "Hit15.wav");
  Sound_Load(&HIT_SOUNDS[16], "Hit16.wav");
  Sound_Load(&HIT_SOUNDS[17], "Hit17.wav");

  Palette_Make(&settings->palette);

  Palette_LoadFromBitmap("tile.png", &settings->palette);
  Bitmap_Load24("tile.png", &SPRITESHEET, 0xFF, 0x00, 0xFF);

  CharacterSrcPalette.count = 4;
  CharacterSrcPalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  CharacterSrcPalette.colours[1] = Make_RGB(0xFF, 0x00, 0x00);
  CharacterSrcPalette.colours[2] = Make_RGB(0x00, 0xFF, 0x00);
  CharacterSrcPalette.colours[3] = Make_RGB(0x00, 0x00, 0xFF);

  PlayerPalette.count = 4;
  PlayerPalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  PlayerPalette.colours[1] = Make_RGB(0x82, 0x2c, 0x2f);
  PlayerPalette.colours[2] = Make_RGB(0xb8, 0x3e, 0x40);
  PlayerPalette.colours[3] = Make_RGB(0xd0, 0x46, 0x48);

  EnemyPalette.count = 4;
  EnemyPalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  EnemyPalette.colours[1] = Make_RGB(0x6c, 0x74, 0x7b);
  EnemyPalette.colours[2] = Make_RGB(0xdc, 0xde, 0xe3);
  EnemyPalette.colours[3] = Make_RGB(0xf0, 0xf8, 0xf7);

  CorpsePalette.count = 4;
  CorpsePalette.colours[0] = Make_RGB(0xFF, 0x00, 0xFF);
  CorpsePalette.colours[1] = Make_RGB(0x32, 0x32, 0x2e);
  CorpsePalette.colours[2] = Make_RGB(0x46, 0x46, 0x48);
  CorpsePalette.colours[3] = Make_RGB(0x4c, 0x4c, 0x4c);

  Bitmap_Load24_PaletteSwap("character.png", &ANIMATIONS[0], 0xFF, 0x00, 0xFF, &CharacterSrcPalette, &PlayerPalette);
  Bitmap_Load24_PaletteSwap("character.png", &ANIMATIONS[1], 0xFF, 0x00, 0xFF, &CharacterSrcPalette, &EnemyPalette);
  Bitmap_Load24_PaletteSwap("character.png", &ANIMATIONS[2], 0xFF, 0x00, 0xFF, &CharacterSrcPalette, &CorpsePalette);

  Font_Load("KageSans.png", &FONT_KAGESANS, Colour_Make(0,0,255), Colour_Make(255,0,255));

  Input_BindKey(SDL_SCANCODE_ESCAPE, CTRL_QUIT);
  Input_BindKey(SDL_SCANCODE_W,      CTRL_MOVE_UP);
  Input_BindKey(SDL_SCANCODE_S,      CTRL_MOVE_DOWN);
  Input_BindKey(SDL_SCANCODE_A,      CTRL_MOVE_LEFT);
  Input_BindKey(SDL_SCANCODE_D,      CTRL_MOVE_RIGHT);
  Input_BindKey(SDL_SCANCODE_SPACE,  CTRL_MOVE_JUMP);
  //Input_BindKey(SDL_SCANCODE_H,      CTRL_CROUCH);
  Input_BindKey(SDL_SCANCODE_J,      CTRL_HIT);
  Input_BindKey(SDL_SCANCODE_K,      CTRL_BLOCK);
  Input_BindKey(SDL_SCANCODE_1,      CTRL_CHEAT);
  Input_BindKey(SDL_SCANCODE_M,      CTRL_MUSIC);

  Level_Load("level1.tmx");

  Music_Play("rage.mod");

}

void Start()
{
}

void Step()
{
  COUNTER_FRAME++;

  if (COUNTER_FRAME == 30)
  {
    COUNTER_FRAME = 0;
    COUNTER_SECOND++;
  }

  if (mode == 0)
    Title();
  else if (mode == 1)
    Game();
  else if (mode == 2)
    Win();
}

void Title()
{
  SDL_Rect src, dst;
  src.w = 128;
  src.h = 128;
  src.x = 288;
  src.y = 0;

  dst.x = 320 / 2 - 128 / 2;
  dst.y = (214 / 2 - 128 / 2);
  dst.w = 128;
  dst.h = 128;

  Level_Splat(0);

  Canvas_Splat3(&SPRITESHEET, &dst, &src);

  if (COUNTER_FRAME >= 15)
  {
    Canvas_PrintStr(80+1, 180+1, &FONT_KAGESANS, 5, "PRESS [S] TO PLAY");
    Canvas_PrintStr(80, 180, &FONT_KAGESANS, 3, "PRESS [S] TO PLAY");
  }

  if (Input_GetActionReleased(CTRL_MOVE_DOWN))
  {
    StartGame();
  }

}

void Win()
{
  SDL_Rect src, dst;
  src.w = 128;
  src.h = 128;
  src.x = 288;
  src.y = 0;

  dst.x = 320 / 2 - 128 / 2;
  dst.y = (214 / 2 - 128 / 2);
  dst.w = 128;
  dst.h = 128;

  Level_Splat(0);

  Canvas_Splat3(&SPRITESHEET, &dst, &src);

  if (COUNTER_FRAME >= 8)
  {
    Canvas_PrintStr(40 + 1, 180 + 1, &FONT_KAGESANS, 5, "CONGRATULATIONS YOU WON!!");
    Canvas_PrintStr(40, 180, &FONT_KAGESANS, 3, "CONGRATULATIONS YOU WON!!");
  }

}

void StartGame()
{
  mode = 1;

  Objects_Setup();
  Level_StartSection(1);
  PLAYER = Objects_FindFirstOf(OT_Player);
  Objects_SetTrackingObjectType(OT_Enemy, PLAYER);

  Canvas_SetFlags(0, CNF_Clear | CNF_Render, 4);
  levelState = 1;
  levelTimer = 0;
}

void Game()
{

  if (Objects_FindFirstOf(OT_Player) == 0)
  {
    Objects_Clear();
    Level_PrevSection();
    PLAYER = Objects_FindFirstOf(OT_Player);
    Objects_SetTrackingObjectType(OT_Enemy, PLAYER);
  }

  Objects_PreTick();

  int nextMovement = 0;

  if (Input_GetActionDown(CTRL_MOVE_LEFT))
    nextMovement = MV_Left;
  else if (Input_GetActionDown(CTRL_MOVE_RIGHT))
    nextMovement = MV_Right;
  
  if (Input_GetActionDown(CTRL_MOVE_UP))
    nextMovement = MV_Up;
  else if (Input_GetActionDown(CTRL_MOVE_DOWN))
    nextMovement = MV_Down;

  int nextAction = 0;

  //if (Input_GetActionReleased(CTRL_HIT))
  //  nextAction = MA_Hit;
  //else 
  //if (Input_GetActionDown(CTRL_CROUCH))
 //   nextAction |= MA_Crouch;
  if (Input_GetActionDown(CTRL_BLOCK))
    nextAction |= MA_Block;
  if (Input_GetActionPressed(CTRL_HIT))
    nextAction |= MA_Hit;

#if 0
  Rect rect;
  rect.left   = 0;
  rect.right  = SCREEN_WIDTH;
  rect.top    = 0;
  rect.bottom = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE;
  Canvas_DrawFilledRectangle(19, rect);

  rect.left   = 0;
  rect.right  = SCREEN_WIDTH;
  rect.top    = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE;
  rect.bottom = SCREEN_HEIGHT;
  Canvas_DrawFilledRectangle(19, rect);

  SDL_Rect roadSrc;
  roadSrc.x = 0;
  roadSrc.y = 16;
  roadSrc.w = 320;
  roadSrc.h = 64;

  SDL_Rect roadDst;
  roadDst.x = 0;
  roadDst.y = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - 64;
  roadDst.w = 320;
  roadDst.h = 64;

  Canvas_Splat3(&SPRITESHEET, &roadDst, &roadSrc);

  SDL_Rect skySrc;
  skySrc.x = 0;
  skySrc.y = 80;
  skySrc.w = 320;
  skySrc.h = 144;

  SDL_Rect skyDst;
  skyDst.x = 0;
  skyDst.y = 0;
  skyDst.w = 320;
  skyDst.h = 144;

  Canvas_Splat3(&SPRITESHEET, &skyDst, &skySrc);

  Canvas_DrawFilledRectangle(19, Make_Rect2(0, 40, 160, 144));
  Canvas_DrawFilledRectangle(19, Make_Rect2(160, 80, 240, 144));
  Canvas_DrawFilledRectangle(19, Make_Rect2(240, 60, 320, 144));
#endif

  if (nextMovement != 0)
  {
    Objects_SetMovementVector(PLAYER, nextMovement);
  }

  if (nextAction != 0)
  {
    Objects_SetMovementAction(PLAYER, nextAction);
  }

  if (levelState == 0)
  {
    levelOffset += 4;
    levelTimer++;

    Level_Draw(levelOffset);
    Objects_Tick(false);
    Objects_Draw(levelOffset);

    if (levelOffset == 320)
    {
      Level_PostNextSection();
      levelState = 1;
    }
  }
  else
  {
    Level_Draw(0);
    Objects_Tick(true);
    Objects_Draw(0);
  }

  if ( Objects_FindFirstOf(OT_Enemy) == 0)
  {
    Objects_ModPositions();
    if (Level_NextSection() == false)
    {
      mode = 2;
      return;
    }

    PLAYER = Objects_FindFirstOf(OT_Player);
    Objects_SetTrackingObjectType(OT_Enemy, PLAYER);
    levelState = 0;
    levelOffset = 0;
    levelTimer = 0;
  }

  if (Input_GetActionReleased(CTRL_CHEAT))
  {
    //Objects_Heal(OT_Player);
    //Objects_KO(OT_Enemy);
  }

  if (Input_GetActionReleased(CTRL_MUSIC))
  {
    Music_PauseToggle();
  }

}

void Sound_PlayHit()
{
  int idx = rand() % 17;
  Sound_Play(&HIT_SOUNDS[idx], RETRO_SOUND_DEFAULT_VOLUME);
}
