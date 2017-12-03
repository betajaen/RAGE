#include "functions.h"

typedef struct {
  u8  length;
  u8  style;
  u8  speed;
  u16 x;
  u16 y;
  u8  direction;
} AnimationInfo;

AnimationInfo kAnimationInfos[] = {
// C  Style    Sp  X  Y
  {1, AS_Once, 30, 0, 0, 1},  // ANIM_Stand
  {5, AS_Loop, 3,  64,0, 1},  // ANIM_Walk
  {2, AS_Once, 5,  0, 96,1},  // ANIM_Punch
  {3, AS_Once, 2,  0, 48,1},  // ANIM_CrouchDown
  {3, AS_Once, 2,  64,48,0 }, // ANIM_CrouchUp
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

inline static u8 NextFrame(u8 frame, u8 direction)
{
  if (direction == 1)
    return frame + 1;
  else
    return frame - 1;
}

inline static u8 FirstFrame(u8 count, u8 direction)
{
  if (direction == 1)
    return 0;
  else
    return count - 1;
}

u8 Animation_FirstFrame(u8 animation)
{
  AnimationInfo* anim = &kAnimationInfos[animation];

  return FirstFrame(anim->length, anim->direction);
}

inline static u8 IsEnded(u8 frame, u8 count, u8 direction)
{
  if (direction == 1)
    return frame == count - 1;
  else
    return frame == 0;
}

bool Animation_IsEnded(u8 frame, u8 animation)
{
  AnimationInfo* anim = &kAnimationInfos[animation];

  return IsEnded(frame, anim->length, anim->direction);
}

void Animation_NextFrame(u8* ticks, u8* frame, u8* ended, u8 animation)
{
  AnimationInfo* anim = &kAnimationInfos[animation];

  switch (anim->style)
  {
    case AS_Once:
    {
      if ((*frame) < anim->length)
      {
        (*ticks) = (*ticks) + 1;

        if ((*ticks) == anim->speed)
        {
          if (IsEnded(*frame, anim->length, anim->direction))
          {
            (*ended) = 1;
          }
          else
          {
            (*frame) = NextFrame((*frame), anim->direction);
            (*ticks) = 0;
            (*ended) = 0;
          }
        }
      }
    }
    break;
    case AS_Loop:
    {
      (*ticks) = (*ticks) + 1;

      if ((*ticks) == anim->speed)
      {
        (*frame) = NextFrame((*frame), anim->direction);
        (*ticks) = 0;
        (*ended) = 0;

        if (IsEnded((*frame), anim->length, anim->direction))
        {
          (*frame) = FirstFrame(anim->length, anim->direction);
        }
      }
    }
    break;
#if 0
    case AS_PingPong:
    {
      if (object->bAnimationState == 1)
      {
        object->bAnimationEnd = 0;

        if (object->frameTicks == object->frameRate)
        {
          object->frameCurrent++;
          object->frameTicks = 0;

          if (object->frameCurrent == object->frameCount)
          {
            object->bAnimationState = 0;
          }
        }

        object->frameTicks++;
      }
      else
      {
        object->bAnimationEnd = 0;

        if (object->frameTicks == object->frameRate)
        {
          object->frameCurrent--;
          object->frameTicks = 0;

          if (object->frameCurrent == object->frameCount)
          {
            object->bAnimationState = 1;
          }
        }

        object->frameTicks++;
      }
    }
    break;
#endif
  }
}
