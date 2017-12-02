#ifndef DATA_H
#define DATA_H

#define RETRO_WINDOW_CAPTION "RAGE"
#define RETRO_ARENA_SIZE Kilobytes(512)
#define RETRO_WINDOW_DEFAULT_WIDTH 1280
#define RETRO_WINDOW_DEFAULT_HEIGHT 720
#define RETRO_CANVAS_DEFAULT_WIDTH (RETRO_WINDOW_DEFAULT_WIDTH / 2)
#define RETRO_CANVAS_DEFAULT_HEIGHT (RETRO_WINDOW_DEFAULT_HEIGHT / 2)

#define SCREEN_WIDTH  (RETRO_CANVAS_DEFAULT_WIDTH)
#define SCREEN_HEIGHT (RETRO_CANVAS_DEFAULT_HEIGHT)
#define SCREEN_BOTTOM_EDGE  (SCREEN_HEIGHT - 256)
#define SCREEN_TOP_EDGE  128

#define CHARACTER_FRAME_W (16 * 2)
#define CHARACTER_FRAME_H (16 * 3)
#define CHARACTER_FRAME_SPRITESHEET_ORIGIN_X (0)
#define CHARACTER_FRAME_SPRITESHEET_ORIGIN_Y (16)


#include "retro.h"

extern Font   FONT_NEOSANS;
extern Bitmap SPRITESHEET;
extern Bitmap ANIMATIONS[2];

extern u32    COUNTER_FRAME;
extern u32    COUNTER_SECOND;

typedef enum
{
  OT_None,
  OT_Player,
  OT_Enemy
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
  MA_Guard  = 3
} MovementAction;

typedef enum
{
  ANIM_Stand = 0,
  ANIM_Walk  = 1,
  ANIM_Punch = 2,
} AnimationType;

typedef enum
{
  AS_Once     = 0,
  AS_Loop     = 1,
  AS_PingPong = 2
} AnimationStyle;

#endif
