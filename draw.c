#include "functions.h"

typedef struct {
  u8  w;
  u8  h;
  u8  length;
  u8  style;
  u8  speed;
  u16 x;
  u16 y;
  u8  direction;
} AnimationInfo;

AnimationInfo kAnimationInfos[] = {
// W   H   Nb Style    Spd  X  Y     Dir
  {32, 48, 1, AS_Once, 30, 0,   0,  1 },  // ANIM_Stand
  {32, 48, 5, AS_Loop, 3,  64,  0,  1 },  // ANIM_Walk
  {32, 48, 2, AS_Once, 5,  0,   96, 1 },  // ANIM_Punch
  {32, 48, 3, AS_Once, 1,  0,   48, 1 },  // ANIM_CrouchDown
  {32, 48, 3, AS_Once, 1,  0,   48, 0 },  // ANIM_CrouchUp
  {32, 48, 1, AS_Once, 1,  224, 48, 1 },  // ANIM_StandBlock
  {32, 48, 1, AS_Once, 1,  224, 96, 1 },  // ANIM_CrouchBlock
  {32, 48, 2, AS_Once, 3,  0,   96, 1 },  // ANIM_StandPunch
  {32, 48, 2, AS_Once, 3,  64,  96, 1 },  // ANIM_CrouchPunch
  {32, 48, 2, AS_Once, 4,  0,  144, 1 },  // ANIM_StandHit
  {32, 48, 2, AS_Once, 4,  64, 144, 1 },  // ANIM_CrouchHit
  {48, 48, 2, AS_Once, 4,  0,  192, 1 },  // ANIM_Death
};

static const u8 kAnimationCount = sizeof(kAnimationInfos) / sizeof(AnimationInfo);

void Draw_Animation(i32 x, i32 y, u8 type, u32 animation, u32 frame, i8 direction, u8 depth)
{
  SDL_Rect src, dst;

  if (animation >= kAnimationCount)
    animation = 0;

  AnimationInfo* info = &kAnimationInfos[animation];

  src.x = info->x + (info->w * frame);
  src.y = info->y;
  src.w = info->w;
  src.h = info->h;

  dst.x = x;
  dst.y = y;
  dst.w = src.w;
  dst.h = src.h;

  u8 r, g, b;

  if (depth >= 3)
    depth = 3;

  switch(depth)
  {
    case 0: r = 0xFF; g = 0xFF; b = 0xFF; break;
    case 1: r = 0xAA; g = 0xAA; b = 0xAA; break;
    case 2: r = 0x88; g = 0x88; b = 0x88; break;
    case 3: r = 0x44; g = 0x44; b = 0x44; break;
  }

  if (direction == 1)
  {
    Canvas_Splat3Colour(&ANIMATIONS[type - 1], &dst, &src, r, g, b);
  }
  else
  {
    Canvas_SplatFlipColour(&ANIMATIONS[type - 1], &dst, &src, SDL_FLIP_HORIZONTAL, r, g, b);
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
