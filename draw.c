#include "functions.h"

static const u8 kAnimationLengths[] = {
  1, // ANIM_Stand
  5, // ANIM_Walk
};

static const u8 kAnimationStyles[] = {
  AS_Once,    // ANIM_Stand
  AS_Loop     // ANIM_Walk
};

static const u8 kAnimationSpeeds[] = {
  30,
  5
};

static const u8 kAnimationCount = sizeof(kAnimationLengths);

void Draw_Animation(i32 x, i32 y, u32 animation, u32 frame, i8 direction)
{
  SDL_Rect src, dst;
  dst.x = x;
  dst.y = y;

  if (animation >= kAnimationCount)
    animation = 0;

  const u8 maxFrame = kAnimationLengths[animation];

  if (frame >= maxFrame)
    frame %= maxFrame;

  src.x = CHARACTER_FRAME_SPRITESHEET_ORIGIN_X + (CHARACTER_FRAME_W * frame);
  src.y = CHARACTER_FRAME_SPRITESHEET_ORIGIN_Y + (CHARACTER_FRAME_H * animation);
  dst.w = CHARACTER_FRAME_W;
  dst.h = CHARACTER_FRAME_H;
  src.w = CHARACTER_FRAME_W;
  src.h = CHARACTER_FRAME_H;

  if (direction == 1)
  {
    Canvas_Splat3(&SPRITESHEET, &dst, &src);
  }
  else
  {
    Canvas_SplatFlip(&SPRITESHEET, &dst, &src, SDL_FLIP_HORIZONTAL);
  }
}

void Animation_GetInfo(u8 type, u8* speed, u8* frameCount, u8* animStyle)
{
  *frameCount = kAnimationLengths[type];
  *animStyle = kAnimationStyles[type];
  *speed = kAnimationSpeeds[type];
}
