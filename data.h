#ifndef DATA_H
#define DATA_H

#define RETRO_WINDOW_CAPTION "RAGE"
#define RETRO_ARENA_SIZE Kilobytes(512)
#define WINDOW_SCALE 3
#define RETRO_WINDOW_DEFAULT_WIDTH  (WINDOW_SCALE * 320)
#define RETRO_WINDOW_DEFAULT_HEIGHT (WINDOW_SCALE * 224)
#define RETRO_CANVAS_DEFAULT_WIDTH (RETRO_WINDOW_DEFAULT_WIDTH   / WINDOW_SCALE)
#define RETRO_CANVAS_DEFAULT_HEIGHT (RETRO_WINDOW_DEFAULT_HEIGHT / WINDOW_SCALE)

#define SCREEN_WIDTH  (RETRO_CANVAS_DEFAULT_WIDTH)
#define SCREEN_HEIGHT (RETRO_CANVAS_DEFAULT_HEIGHT)

#define CHARACTER_FRAME_W (16 * 2)
#define CHARACTER_FRAME_H (16 * 3)
#define CHARACTER_FRAME_SPRITESHEET_ORIGIN_X (0)
#define CHARACTER_FRAME_SPRITESHEET_ORIGIN_Y (16)
#define SCREEN_BOTTOM_EDGE 16

#include "retro.h"


typedef enum
{
  OT_None = 0,
  OT_Player = 1,
  OT_Enemy = 2,
  OT_Corpse = 3,


  OT_COUNT = 3,
} ObjectType;

typedef enum
{
  DIR_Left  =  0,
  DIR_Right =  1
} DirectionType;

typedef enum
{
  MV_Left  = 1,
  MV_Right = 2,
  MV_Up    = 4,
  MV_Down  = 8
} MovementVector;

typedef enum
{
  MA_Hit    = 1,
  MA_Crouch = 2,
  MA_Block  = 4
} MovementAction;

typedef enum
{
  ANIM_Stand       = 0,
  ANIM_Walk        = 1,
  ANIM_Punch       = 2,
  ANIM_CrouchDown  = 3,
  ANIM_CrouchUp    = 4,
  ANIM_StandBlock  = 5,
  ANIM_CrouchBlock = 6,
  ANIM_StandPunch  = 7,
  ANIM_CrouchPunch = 8,
  ANIM_StandHit    = 9,
  ANIM_CrouchHit   = 10,
  ANIM_Death       = 11
} AnimationType;

typedef enum
{
  AS_Once     = 0,
  AS_Loop     = 1,
  AS_PingPong = 2
} AnimationStyle;

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
  CTRL_BLOCK,
  CTRL_CHEAT
} Control;

typedef struct
{
  i32 x, y, halfWidth, halfHeight;
} HitboxTest;

typedef struct
{
  Point position, normal, delta;
} HitboxResult;

typedef struct
{
  i32 x0, y0, x1, y1;
} Hitbox;


extern Font   FONT_NEOSANS;
extern Bitmap SPRITESHEET;
extern Bitmap ANIMATIONS[OT_COUNT];

extern u32    COUNTER_FRAME;
extern u32    COUNTER_SECOND;

#endif
