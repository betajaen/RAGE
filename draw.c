#include "functions.h"

typedef struct {
  u8  length;
  u8  style;
  u8  speed;
  u16 x;
  u16 y;
} AnimationInfo;

AnimationInfo kAnimationInfos[] = {
// C  Style    Sp  X  Y
  {1, AS_Once, 30, 0, 0},   // ANIM_Stand
  {5, AS_Loop, 3,  64,0},   // ANIM_Walk
  {2, AS_Once, 5,  0, 96},  // ANIM_Punch
  {3, AS_Once, 5,  64,96},  // ANIM_Crouch
};


static const u8 kAnimationCount = 4;

void Draw_Animation(i32 x, i32 y, u8 type, u32 animation, u32 frame, i8 direction)
{
  SDL_Rect src, dst;
  dst.x = x;
  dst.y = y;

  if (animation >= kAnimationCount)
    animation = 0;

  const u8 maxFrame = kAnimationInfos[animation].length;

  if (frame >= maxFrame)
    frame %= maxFrame;

  const int animationX = kAnimationInfos[animation].x;
  const int animationY = kAnimationInfos[animation].y;

  src.x = animationX + (CHARACTER_FRAME_W * frame);
  src.y = animationY;
  dst.w = CHARACTER_FRAME_W;
  dst.h = CHARACTER_FRAME_H;
  src.w = CHARACTER_FRAME_W;
  src.h = CHARACTER_FRAME_H;

  if (direction == 1)
  {
    Canvas_Splat3(&ANIMATIONS[type - 1], &dst, &src);
  }
  else
  {
    Canvas_SplatFlip(&ANIMATIONS[type - 1], &dst, &src, SDL_FLIP_HORIZONTAL);
  }
}

void Animation_GetInfo(u8 type, u8* speed, u8* frameCount, u8* animStyle)
{
  *frameCount = kAnimationInfos[type].length;
  *animStyle  = kAnimationInfos[type].style;
  *speed      = kAnimationInfos[type].speed;
}
