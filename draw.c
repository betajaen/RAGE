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
  {1, AS_Once, 30, 0,   0,  1 },  // ANIM_Stand
  {5, AS_Loop, 3,  64,  0,  1 },  // ANIM_Walk
  {2, AS_Once, 5,  0,   96, 1 },  // ANIM_Punch
  {3, AS_Once, 1,  0,   48, 1 },  // ANIM_CrouchDown
  {3, AS_Once, 1,  0,   48, 0 },  // ANIM_CrouchUp
  {1, AS_Once, 1,  224, 48, 1 },  // ANIM_StandBlock
  {1, AS_Once, 1,  224, 96, 1 },  // ANIM_CrouchBlock
  {2, AS_Once, 8,  0,   96, 1 },  // ANIM_StandPunch
  {2, AS_Once, 8,  64,  96, 1,},  // ANIM_CrouchPunch
};


static const u8 kAnimationCount = sizeof(kAnimationInfos) / sizeof(AnimationInfo);

void Draw_Animation(i32 x, i32 y, u8 type, u32 animation, u32 frame, i8 direction)
{
  SDL_Rect src, dst;
  dst.x = x;
  dst.y = y;

  if (animation >= kAnimationCount)
    animation = 0;

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

inline static u8 LastFrame(u8 count, u8 direction)
{
  if (direction == 1)
    return count - 1;
  else
    return 0;
}

u8 Animation_LastFrame(u8 animation)
{
  AnimationInfo* anim = &kAnimationInfos[animation];

  return LastFrame(anim->length, anim->direction);
}

inline static u8 IsEnded(u8 frame, u8 ticks, u8 maxTicks, u8 count, u8 direction)
{
  if (ticks != maxTicks)
    return false;

  if (direction == 1)
    return frame == count - 1;
  else
    return frame == 0;
}

bool Animation_IsEnded(u8 frame, u8 ticks, u8 animation)
{
  AnimationInfo* anim = &kAnimationInfos[animation];

  return IsEnded(frame, ticks, anim->speed, anim->length, anim->direction);
}

u8  Animation_Speed(u8 animation)
{
  AnimationInfo* anim = &kAnimationInfos[animation];

  return anim->speed - 1;

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
          if (IsEnded(*frame, *ticks, anim->speed, anim->length, anim->direction))
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
        if (IsEnded((*frame), *ticks, anim->speed, anim->length, anim->direction))
        {
          (*frame) = FirstFrame(anim->length, anim->direction);
          (*ended) = 1;
          (*ticks) = 0;
        }
        else
        {
          (*frame) = NextFrame((*frame), anim->direction);
          (*ticks) = 0;
          (*ended) = 0;
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
