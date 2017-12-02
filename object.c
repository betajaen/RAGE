#include "functions.h"

#define MAX_OBJECTS 256

typedef enum 
{
  MS_Walk,
  MS_Crouch,
  MS_Hit,
  MS_Block,
  MS_Jump,
  MS_Damaged,
  MS_KO
} MoveState;

typedef struct
{
  i32 x, w;
  i32 sx, sy;
  u8  type;
  u8  height;
  u8  depth;
  u8  moveDelta;
  u8  moveSpeed;
  u8  moveState;

  u8  frameAnimation;
  u8  frameCurrent;
  u8  frameTicks;
  u8  frameCount;
  u8  frameRate;
  i8  velocity;

  u32 bAnimationState     : 1;
  u32 bDirection          : 1;
  u32 frameAnimationStyle : 2;

} Object;

Object  sObjects[256];
Object* sDrawOrder[256];


static void Object_Tick(Object* object);
static void Object_Draw(Object* object);
static void Object_Initialise(Object* object, u8 type);
static void Object_Clear(Object* object);
static void Object_SetMoveDelta(Object* object, u8 moveVector);

void Objects_Setup()
{
  SDL_memset(&sObjects, 0, sizeof(sObjects));
}

void Objects_Teardown()
{
  SDL_memset(&sObjects, 0, sizeof(sObjects));
}

u16  Objects_Create(u8 type)
{
  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    Object* object = &sObjects[i];
    if (object->type == 0)
    {
      Object_Initialise(object, type);

      return 1 + i;
    }
  }

  return 0;
}

void Objects_Destroy(u16 id)
{
  if (id != 0)
  {
    Object_Clear(&sObjects[id - 1]);
  }
}

void Objects_Tick()
{
  for(int i=0;i < MAX_OBJECTS;i++)
  {
    Object* object = &sObjects[i];
    if (object->type != 0)
    {
      Object_Tick(object);
    }
  }
}

void Objects_Draw()
{
  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    Object* object = &sObjects[i];
    if (object->type != 0)
    {
      Object_Draw(object);
    }
  }
}

void Objects_SetMovementVector(u16 id, u8 movementVector)
{
  if (id != 0)
  {
    Object_SetMoveDelta(&sObjects[id - 1], movementVector);
  }
}

#define DRAG 1

static void Object_ResetAnim(Object* object, u8 anim)
{
  object->frameAnimation = anim;
  object->frameTicks = 0;
  u8 style = object->frameAnimationStyle;

  Animation_GetInfo(object->frameAnimation, &object->frameRate, &object->frameCount, &style);
  object->frameAnimationStyle = style;
}

static void Object_Tick(Object* object)
{
  u8 newDirection = object->bDirection;
  bool resetAnim  = false;
  bool movingAnim = false;

  i8 velocity = object->velocity;

  if (velocity > 0)
    velocity -= DRAG;
  else if (velocity < 0)
    velocity += DRAG;

  if ((object->moveDelta & MV_Left) != 0)
  {
    velocity -= object->moveSpeed;
    newDirection = DIR_Left;
  }

  if ((object->moveDelta & MV_Right) != 0)
  {
    velocity += object->moveSpeed;
    newDirection = DIR_Right;
  }

  if ((object->moveDelta & MV_Up) != 0)
  {
    if (object->depth < 32)
      object->depth++;
  }

  if ((object->moveDelta & MV_Down) != 0)
  {
    if (object->depth > 0)
      object->depth--;
  }

  object->bDirection = newDirection;
  
  if (object->moveState == MS_Walk)
  {
    if (velocity == 0 && object->velocity != 0)
    {
      // Stopped
      Object_ResetAnim(object, ANIM_Stand);
    }
    else if (velocity !=0 && object->velocity == 0)
    {
      // Moving
      Object_ResetAnim(object, ANIM_Walk);
    }
  }
  else if (object->moveState == MS_Crouch)
  {
    
  }
  else if (object->moveState == MS_Hit)
  {

  }
  else if (object->moveState == MS_Block)
  {

  }
  else if (object->moveState == MS_Crouch)
  {

  }
  else if (object->moveState == MS_Jump)
  {

  }
  else if (object->moveState == MS_Damaged)
  {

  }
  else if (object->moveState == MS_KO)
  {

  }

  object->velocity  = velocity;
  object->x += object->velocity;
  object->moveDelta = 0;

  if (object->depth > 32)
    object->depth = 32;

  object->sx = object->x;  // (For now doesn't include screen scrolling, clipping, etc.)
  object->sy = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->depth * 4);

  switch(object->frameAnimationStyle)
  {
    case AS_Once:
    {
      if (object->frameCurrent < object->frameCount)
      {
        object->frameTicks++;

        if (object->frameTicks == object->frameRate)
        { 
          object->frameCurrent++;
          object->frameTicks = 0;
        }
      }
    }
    break;
    case AS_Loop:
    {
      object->frameTicks++;

      if (object->frameTicks == object->frameRate)
      {
        object->frameCurrent++;
        object->frameTicks = 0;

        if (object->frameCurrent == object->frameCount)
          object->frameCurrent = 0;
      }
    }
    break;
    case AS_PingPong:
    {
      if (object->bAnimationState == 1)
      {
        object->frameTicks++;

        if (object->frameTicks == object->frameRate)
        {
          object->frameCurrent++;
          object->frameTicks = 0;

          if (object->frameCurrent == object->frameCount)
          {
            object->bAnimationState = 0;
          }
        }
      }
      else
      {
        object->frameTicks++;

        if (object->frameTicks == object->frameRate)
        {
          object->frameCurrent--;
          object->frameTicks = 0;

          if (object->frameCurrent == object->frameCount)
          {
            object->bAnimationState = 1;
          }
        }
      }
    }
    break;
  }
  
}


static void Object_SetMoveDelta(Object* object, u8 moveVector)
{
  object->moveDelta = moveVector;
}

static void Object_Draw(Object* object)
{
  Draw_Animation(object->sx, object->sy - CHARACTER_FRAME_H, object->frameAnimation, object->frameCurrent, object->bDirection);
}

static void Object_Initialise(Object* object, u8 type)
{
  SDL_memset(object, 0, sizeof(Object));
  
  object->type = type;
  object->moveSpeed = 1;
}

static void Object_Clear(Object* object)
{
  object->type = OT_None;
}
