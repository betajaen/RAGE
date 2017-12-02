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

  u16 trackingObject;
  u8  trackingTimer;

  u32 bAnimationState     : 1;
  u32 bDirection          : 1;
  u32 bIsHitting          : 1;
  u32 bIsCrouched         : 1;
  u32 bIsGuarding         : 1;
  u32 frameAnimationStyle : 2;

} Object;

Object  sObjects[256];
Object* sDrawOrder[256];

u8      sGridSize[(64 * 16)];

static void Object_PreTick(Object* object);
static void Object_Tick(Object* object);
static void Object_Draw(Object* object);
static void Object_Initialise(Object* object, u8 type);
static void Object_Clear(Object* object);
static void Object_SetMoveDelta(Object* object, u8 moveVector);
static void Object_SetMoveAction(Object* object, u8 moveAction);
static void Object_SetPosition(Object* object, i32 x, u8 depth);

static inline u32 GetGridCoord(i32 x, u8 depth)
{
  x /= 10;

  if (x >= 64)
    x = 63;
  else if (x < 0)
    x = 0;
  
  if (depth < 0)
    depth = 0;
  else if (depth > 16)
    depth = 16;

  return x + (depth * 64);
}

static inline bool CheckDepth(i32 x, u8 depth)
{
  return sGridSize[GetGridCoord(x, depth)] == 0;
}

static inline u8 GetDepth(i32 x, u8 depth)
{
  return sGridSize[GetGridCoord(x, depth)];
}

static inline void MarkDepth(i32 x, u8 depth)
{
  sGridSize[GetGridCoord(x, depth)]++;
}

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

void Objects_PreTick()
{
  for (int i = 0; i < (64 * 16); i++)
  {
    sGridSize[i] = 0;
  }

  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    Object* object = &sObjects[i];
    if (object->type != 0)
    {
      Object_PreTick(object);
    }
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

  for(int i=0;i < 64;i++)
  {
    for(int j=0;j < 16;j++)
    {
      int x = i * 10;
      int y = SCREEN_TOP_EDGE + (j * 8);
      Canvas_PrintF(x, y, &FONT_NEOSANS, 18, "%i", GetDepth(i * 10, 15 - j));
    }
  }

}

void Objects_SetPosition(u16 id, i32 x, u8 depth)
{
  if (id != 0)
  {
    Object_SetPosition(&sObjects[id - 1], x, depth);
  }
}

void Objects_SetMovementVector(u16 id, u8 movementVector)
{
  if (id != 0)
  {
    Object_SetMoveDelta(&sObjects[id - 1], movementVector);
  }
}

void Objects_SetMovementAction(u16 id, u8 movementAction)
{
  if (id != 0)
  {
    Object_SetMoveAction(&sObjects[id - 1], movementAction);
  }
}

void Objects_SetTrackingObject(u16 id, u16 other)
{
  if (id != 0)
  {
    Object* object = &sObjects[id - 1];
    object->trackingObject = other;
    object->trackingTimer = 8;
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

  if (velocity < -4)
    velocity = -4;
  else if (velocity > 4)
    velocity =  4;

  if (object->trackingObject != 0)
  {
    object->trackingTimer--;

    if (object->trackingTimer == 0)
    {
      Object* other = &sObjects[object->trackingObject - 1];

      object->trackingTimer = 1 + rand() % 3;

      int distanceX = (other->x - object->x);
      int distanceY = (other->depth - object->depth);
      int distanceSq = (distanceX * distanceX) + (distanceY * distanceY);

      if (distanceSq < (125 * 125))
      {

        int depthDiff = (object->depth - other->depth);

        if (depthDiff > 2)
        {
          if (CheckDepth(object->x, object->depth - 1))
          {
            object->moveDelta |= MV_Down;
          }
        }
        else if (depthDiff < 2)
        {
          if (CheckDepth(object->x, object->depth + 1))
          {
            object->moveDelta |= MV_Up;
          }
        }

        if (object->x < other->x)
        {
          if (CheckDepth(object->x + 11, object->depth))
          {
            object->moveDelta |= MV_Right;
          }
        }
        else if (object->x > other->x)
        {
          if (CheckDepth(object->x - 11, object->depth))
          {
            object->moveDelta |= MV_Left;
          }
        }

      }
      
    }
  }

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


  if (object->bIsCrouched == MS_Crouch)
  {

  }
  else if (object->moveState == MS_Walk)
  {
    if (velocity == 0 && object->velocity != 0)
    {
      // Stopped
      Object_ResetAnim(object, ANIM_Stand);
      object->moveSpeed = 1;
    }
    else if (velocity !=0 && object->velocity == 0)
    {
      // Moving
      Object_ResetAnim(object, ANIM_Walk);
      object->moveSpeed = 2;
    }
  }
  else if (object->moveState == MS_Hit)
  {

  }
  else if (object->moveState == MS_Block)
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

  if (object->depth > 16)
    object->depth = 16;

  object->sx = object->x;  // (For now doesn't include screen scrolling, clipping, etc.)
  object->sy = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->depth * 8);

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

static void Object_PreTick(Object* object)
{
  if (object->type == OT_Enemy)
  {
    MarkDepth(object->x, object->depth);
  }
  else if (object->type == OT_Player)
  {
    object->bIsCrouched = false;
  }
}

static void Object_SetPosition(Object* object, i32 x, u8 depth)
{
  object->x = x;
  object->depth = depth;
}

static void Object_SetMoveDelta(Object* object, u8 moveVector)
{
  object->moveDelta = moveVector;
}

static void Object_SetMoveAction(Object* object, u8 moveAction)
{
  object->bIsCrouched = ((moveAction & MA_Crouch) != 0);
  object->bIsGuarding = ((moveAction & MA_Guard) != 0);
  
  // object->bIsHitting  = ((moveAction & MA_Hit) != 0);
}

static void Object_Draw(Object* object)
{
  Draw_Animation(object->sx, object->sy - CHARACTER_FRAME_H, object->type, object->frameAnimation, object->frameCurrent, object->bDirection);
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
