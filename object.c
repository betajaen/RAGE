#include "functions.h"

#define MAX_OBJECTS 256
#define SCALE 100

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
  i16 depth;
  u8  moveDelta;
  u8  moveSpeedX;
  u8  moveSpeedY;
  u8  moveState;

  u8  frameAnimation;
  u8  frameCurrent;
  u8  frameTicks;
  u8  frameCount;
  u8  frameRate;
  u8  trackingTimer;

  i16 velocityX;
  i16 velocityY;
  u16 trackingObject;

  u32 bAnimationState     : 1;
  u32 bDirection          : 1;
  u32 bIsHitting          : 1;
  u32 bIsCrouched         : 1;
  u32 bIsGuarding         : 1;
  u32 frameAnimationStyle : 2;

} Object;

Object  sObjects[256];
Object* sDrawOrder[256];

static void Object_PreTick(Object* object);
static void Object_Tick(Object* object);
static void Object_Draw(Object* object);
static void Object_Initialise(Object* object, u8 type);
static void Object_Clear(Object* object);
static void Object_SetMoveDelta(Object* object, u8 moveVector);
static void Object_SetMoveAction(Object* object, u8 moveAction);
static void Object_SetPosition(Object* object, i32 x, u8 depth);


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
  
  i16 velocityX = object->velocityX;
  i16 velocityY = object->velocityY;

  if (velocityX > 0)
    velocityX -= 50;
  else if (velocityX < 0)
    velocityX += 50;

  if (velocityX < -4 * SCALE)
    velocityX = -400;
  else if (velocityX > 4 * SCALE)
    velocityX =  400;

  if (velocityY > 0)
    velocityY -= 50;
  else if (velocityY < 0)
    velocityY += 50;

  if (velocityY < -4 * SCALE)
    velocityY = -400;
  else if (velocityY > 4 * SCALE)
    velocityY = 400;

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
            object->moveDelta |= MV_Down;

        }
        else if (depthDiff < 2)
        {
            object->moveDelta |= MV_Up;
        }

        if (object->x < other->x)
        {
            object->moveDelta |= MV_Right;
        }
        else if (object->x > other->x)
        {
            object->moveDelta |= MV_Left;
        }

      }
      
    }
  }

  if ((object->moveDelta & MV_Left) != 0)
  {
    velocityX -= object->moveSpeedX;
    newDirection = DIR_Left;
  }

  if ((object->moveDelta & MV_Right) != 0)
  {
    velocityX += object->moveSpeedX;
    newDirection = DIR_Right;
  }

  if ((object->moveDelta & MV_Up) != 0)
  {
    velocityY += object->moveSpeedY;
  }

  if ((object->moveDelta & MV_Down) != 0)
  {
    velocityY -= object->moveSpeedY;
  }

  object->bDirection = newDirection;


  if (object->bIsCrouched == MS_Crouch)
  {

  }
  else if (object->moveState == MS_Walk)
  {
    if (velocityX == 0 && object->velocityX != 0 || 
        velocityY == 0 && object->velocityY != 0)
    {
      // Stopped
      Object_ResetAnim(object, ANIM_Stand);
    }
    else if (velocityX != 0 && object->velocityX == 0 || 
             velocityY != 0 && object->velocityY == 0)
    {
      // Moving
      Object_ResetAnim(object, ANIM_Walk);
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

  object->moveDelta = 0;

  object->velocityX  = velocityX;
  object->x += object->velocityX;
  object->velocityY  = velocityY;
  object->depth += object->velocityY;
  
  if (object->depth > 6400)
    object->depth = 6400;
  else if (object->depth < 0)
    object->depth = 0;

  object->sx = object->x / SCALE;  // (For now doesn't include screen scrolling, clipping, etc.)
  object->sy = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->depth / SCALE);

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
//    MarkDepth(object->x, object->depth);
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
  object->moveSpeedX = 100;
  object->moveSpeedY = 100;
}

static void Object_Clear(Object* object)
{
  object->type = OT_None;
}
