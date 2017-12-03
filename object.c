#include "functions.h"

#define MAX_OBJECTS 20
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
  i32 sx, sy;
  u8  type;
  u8  height;
  u8  moveSpeedX;
  u8  moveSpeedY;
  u8  moveState;
  u8  moveFlags;

  u8  frameAnimation;
  u8  frameCurrent;
  u8  frameTicks;
  u8  trackingTimer;
  u8  hitTimer;
  u8  hitState;
  u8  damageTimer;
  u8  lastDamageTime;
  u8  hp;
  u8  aiHitTimer;
  i32 aiSoftTargetX;
  i32 aiSoftTargetY;
  u8  aiSoftTargetTimer;

  i32 x;
  i32 y;
  i16 velocityX;
  i16 velocityY;
  i16 accelerationX;
  i16 accelerationY;

  u16 trackingObject;
  
  u32 bDirection                 : 1;
  u32 bIsHitting                 : 1;
  u32 bIsCrouched                : 1;
  u32 bIsBlocking                : 1;
  u32 bIsBeingDamaged            : 1;
  u32 bIsDead                    : 1;
  u32 bIsDazed                   : 1;
  u32 bAiIsHead                  : 1;
  u32 bAiStayDistance            : 1;
  u32 bFrameAnimationEnded       : 1;

  Hitbox bounds, boundsHit;
  
} Object;

Object  sObjects[256];
Object* sDrawOrder[256];

static i32 ClampPosition(i32 position, i16* velocity, i32 min, i32 max)
{
  if (position < min)
  {
    position = min;
    *velocity = 0;
  }
  else if (position > max)
  {
    position = max;
    *velocity = 0;
  }
  return position;
}

static inline bool PointInHitbox(Hitbox* larger, i32 x, i32 y)
{
  return (x >= larger->x0 && x <= larger->x1 &&
          y >= larger->y0 && y <= larger->y1);
}

static inline bool CanMoveForAcceleration(Object* object)
{
  return object->moveState == MS_Walk &&
         !object->bIsCrouched &&
         !object->bIsBlocking;
}

static inline bool IsMovingX(Object* object)
{
  return object->velocityX != 0;
}

static inline bool IsMovingY(Object* object)
{
  return object->velocityY != 0;
}

static inline bool IsNotReallyMovingX(Object* object)
{
  return abs(object->velocityX) < 25;
}

static inline bool IsNotReallyMovingY(Object* object)
{
  return abs(object->velocityY) < 25;
}

static inline bool IsMoving(Object* object)
{
  return IsMovingX(object) || IsMovingY(object);
}

static inline bool IsNotReallyMoving(Object* object)
{
  return IsNotReallyMovingX(object) || IsNotReallyMovingY(object);
}

static inline bool IsReallyCrouching(Object* object)
{
  return !!object->bIsCrouched && object->frameAnimation == ANIM_CrouchDown && !!object->bFrameAnimationEnded;
}


static void Object_PreTick(Object* object);
static void Object_Tick(Object* object);
static void Object_Draw(Object* object);
static void Object_Initialise(Object* object, u8 type);
static void Object_Clear(Object* object);
static void Object_SetMoveDelta(Object* object, u8 moveVector);
static void Object_SetMoveAction(Object* object, u8 moveAction);
static void Object_SetPosition(Object* object, i32 x, u16 y);

void GroupEnemyObject_Tick();

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

  GroupEnemyObject_Tick();
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

void Objects_SetPosition(u16 id, i32 x, u16 y)
{
  if (id != 0)
  {
    Object_SetPosition(&sObjects[id - 1], x, y);
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

static void Object_ResetAnim(Object* object, u8 anim)
{
  object->frameAnimation = anim;
  object->frameCurrent = Animation_FirstFrame(anim);
  object->frameTicks = 0;
  object->bFrameAnimationEnded = 0;
}

static void Object_ResetAnimEnd(Object* object, u8 anim)
{
  object->frameAnimation = anim;
  object->frameCurrent = Animation_LastFrame(anim);
  object->frameTicks = Animation_Speed(anim) - 1;
  object->bFrameAnimationEnded = 1;
}

i32 SolveVelocity(i32 velocity, i32 acceleration, i32 drag, i32 maxVelocity)
{
  if (acceleration != 0)
  {
    velocity += acceleration;
  }
  else if (drag > 0)
  {
    if (velocity - drag > 0)
      velocity -= drag;
    else if (velocity + drag < 0)
      velocity += drag;
    else
      velocity = 0;
  }

  if (velocity > maxVelocity)
    velocity = maxVelocity;
  else if (velocity < -maxVelocity)
    velocity = -maxVelocity;
  return velocity;
}

static void GroupEnemyObject_Tick()
{
  // See if there is a head, if not. Assign first.
  // Others should tick down and move to a random spot around target.

  Object* head = NULL;
  Object* player = NULL;

  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    Object* object = &sObjects[i];
    if (object->type != OT_Player || object->bIsDead)
      continue;
    player = object;
    break;
  }

  if (player == NULL)
    return;

  for(int i=0;i < MAX_OBJECTS;i++)
  {
    Object* object = &sObjects[i];
    if (object->type != OT_Enemy || object->bIsDead)
      continue;
    
    if (object->bAiIsHead)
    {
      head = object;
      break;
    }
  }

  // No head? We assign one, and with the others, we pick a target around the player.
  if (head == NULL)
  {
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
      Object* object = &sObjects[i];
      if (object->type != OT_Enemy || object->bIsDead)
        continue;

      if (head == NULL)
      {
        object->bAiIsHead = 1;
        head = object;
      }
      else
      {
        object->bAiIsHead = 0;

        object->aiSoftTargetX = ((rand() % 300)) * 100;
        object->aiSoftTargetY = ((rand() % 64)) * 100;

        if (object->aiSoftTargetX < 0)
          object->aiSoftTargetX = 0;

        if (object->aiSoftTargetY < 0)
          object->aiSoftTargetY = 0;
        else if (object->aiSoftTargetY > 6400)
          object->aiSoftTargetY = 6400;

        object->aiSoftTargetTimer = 1 + rand() % (30 * 8);
      }
    }
  }
  else
  {
    for (int i = 0; i < MAX_OBJECTS; i++)
    {
      Object* object = &sObjects[i];
      if (object->type != OT_Enemy || object->bIsDead)
        continue;
      if (object == head)
      {
      }
      else
      {
        object->aiSoftTargetTimer--;

        if (object->aiSoftTargetTimer == 0)
        {
          object->aiSoftTargetTimer = 1 + rand() % 15;

          object->aiSoftTargetX = ((rand() % 300)) * 100;
          object->aiSoftTargetY = ((rand() % 64)) * 100;

          if (object->aiSoftTargetX < 0)
            object->aiSoftTargetX = 0;

          if (object->aiSoftTargetY < 0)
            object->aiSoftTargetY = 0;
          else if (object->aiSoftTargetY > 6400)
            object->aiSoftTargetY = 6400;

          object->aiSoftTargetTimer = 1 + rand() % (30 * 8);

        }

      }
    }
  }

}

#define SQ_PX(X) ((X * X) * 100)

static inline MovementVector MaybeOpposite(bool cond, MovementVector dt)
{
  if (!cond)
    return dt;
  switch (dt)
  {
  case MV_Up:   return MV_Down;
  case MV_Down:  return MV_Up;
  case MV_Left:  return MV_Right;
  case MV_Right: return MV_Left;
  }
  return dt;
}

static void EnemyObject_Tick(Object* object)
{

  if (object->trackingObject != 0)
  {
    object->trackingTimer--;

    if (object->trackingTimer == 0)
    {
      int distanceX = 0, distanceY = 0;

      object->trackingTimer = 1 + rand() % 3;
     
      if (object->bAiIsHead)
      {
        Object* other = &sObjects[object->trackingObject - 1];

        distanceX = (other->x - object->x);
        distanceY = (other->y - object->y);
      }
      else
      {
        distanceX = (object->aiSoftTargetX - object->x);
        distanceY = (object->aiSoftTargetY - object->y);
      }
      
      int distanceSq = (distanceX * distanceX) + (distanceY * distanceY);

      bool shouldMove = true; // (distanceSq < SQ_PX(150) && distanceSq > SQ_PX(80));
      bool moveAway = false;
       
      if (object->bAiStayDistance == 0)
        shouldMove = true;
      
      if (object->bAiIsHead && distanceSq < SQ_PX(60))
      {
        Object* other = &sObjects[object->trackingObject - 1];

        HitboxResult result;
        if (Collision_BoxVsBox(&result, &object->bounds, &other->bounds))
        {
          shouldMove = true;
          distanceX = (result.position.x - object->x);
          distanceY = (result.position.y - object->y);
        }
        else
        {
          shouldMove = true;
          moveAway = true;
        }
      }
      else if (object->bAiIsHead == false && distanceSq < SQ_PX(60))
      {
        shouldMove = false;
        moveAway = false;
      }

      if (shouldMove)
      {

        if (distanceY < 0)
        {
          object->moveFlags |= MaybeOpposite(moveAway, MV_Down);
        }
        else if (distanceY > 0)
        {
          object->moveFlags |= MaybeOpposite(moveAway, MV_Up);
        }

        if (distanceX > 0)
        {
          object->moveFlags |= MaybeOpposite(moveAway, MV_Right);
        }
        else if (distanceX < 0)
        {
          object->moveFlags |= MaybeOpposite(moveAway, MV_Left);
        }

      }

      if (object->bAiIsHead && distanceSq < SQ_PX(40))
      {
        object->aiHitTimer--;

        if (object->aiHitTimer == 0)
        {
          if (object->bIsDazed == false)
          {
            object->bIsHitting = true;
          }

          object->aiHitTimer = 64;
        }
        else
        {
          if (object->bIsDazed)
          {
            object->bIsHitting = false;
          }
        }

      }

    }
  }
}

static void MoveFlags2Acceleration(Object* object)
{
  if (CanMoveForAcceleration(object) == false)
    return;

  if ((object->moveFlags & MV_Left) != 0)
  {
    object->accelerationX = -object->moveSpeedX;
  }

  if ((object->moveFlags & MV_Right) != 0)
  {
    object->accelerationX = object->moveSpeedX;
  }

  if ((object->moveFlags & MV_Up) != 0)
  {
    object->accelerationY = object->moveSpeedY;
  }

  if ((object->moveFlags & MV_Down) != 0)
  {
    object->accelerationY = -object->moveSpeedY;
  }
}

static void Object_Tick(Object* object)
{

  i16 velocityX = object->velocityX;
  i16 velocityY = object->velocityY;
  bool wasMoving = IsMoving(object);

  object->accelerationX = 0;
  object->accelerationY = 0;

  if (!object->bIsDead)
  {
    if (object->lastDamageTime > 0)
    {
      object->lastDamageTime--;

      if (object->lastDamageTime == 0)
        object->bIsDazed = false;
    }
    
    if (object->type == OT_Enemy)
    {
      EnemyObject_Tick(object);
    }

    if (object->bIsBeingDamaged)
    {
      object->damageTimer--;
      if (object->damageTimer == 0)
      {
        object->bIsBeingDamaged = 0;

        if (object->hp == 0)
        {
          object->bIsDead = true;
          Object_ResetAnim(object, ANIM_Death);
          object->type = OT_Corpse;
          object->bAiIsHead = 0;
          printf("** DEAD!\n");
        }
        else
        {
          object->lastDamageTime = 4;
          object->bIsDazed = true;
          Object_ResetAnim(object, ANIM_Stand);
        }

        printf("** Damage End %i\n", object->hp);
      }
      else
      {
        if (object->bDirection == 1)
          object->accelerationX -= rand() % 6;
        else
          object->accelerationX += rand() % 6;

        object->accelerationY += (rand() % 6) - 3;
      }
    }
    else
    {
      if (object->bIsCrouched)
      {
        if (
            object->bIsBlocking == false && 
            object->bIsHitting  == false
           )
        {
          if (object->frameAnimation != ANIM_CrouchDown && 
              object->frameAnimation != ANIM_CrouchBlock)
          {
            if (IsNotReallyMoving(object))
            {
              Object_ResetAnim(object, ANIM_CrouchDown);
              object->moveState = MS_Crouch;
              object->velocityX = 0;
              object->velocityY = 0;


              printf("** Down\n");
            }
          }
        }
      }
      else
      {
        if (object->bIsBlocking == false &&
            object->bIsHitting == false)
        {
          if (object->frameAnimation == ANIM_CrouchDown)
          {
            Object_ResetAnim(object, ANIM_CrouchUp);
          }

          if (object->frameAnimation == ANIM_CrouchUp && 
              Animation_IsEnded(object->frameCurrent, object->frameTicks, object->frameAnimation))
          {
            Object_ResetAnim(object, ANIM_Stand);
            object->moveState = MS_Walk;

            printf("** Up\n");

          }
        }
      }

      if (
          object->bIsBlocking && 
          !object->bIsHitting)
      {
        if (
          (object->frameAnimation != ANIM_StandBlock &&
           object->frameAnimation != ANIM_CrouchBlock))
        {
          if (object->frameAnimation == ANIM_CrouchDown)
          {
            Object_ResetAnim(object, ANIM_CrouchBlock);

            printf("** Block Crouch\n");
          }
          else
          {
            Object_ResetAnim(object, ANIM_StandBlock);

            printf("** Block Stand\n");
          }
        }
      }
      else
      {
        if (object->frameAnimation == ANIM_StandBlock || object->frameAnimation == ANIM_CrouchBlock)
        {
          if (object->frameAnimation == ANIM_CrouchBlock)
          {
            Object_ResetAnimEnd(object, ANIM_CrouchDown);

            printf("** Block End Crouch\n");
          }
          else
          {
            Object_ResetAnim(object, ANIM_Stand);
            printf("** Block End Stand\n");
          }
        }
      }

      if ( object->bIsHitting && 
           object->bIsBlocking == false)
      {
        if (object->hitState == 0)
        {
          if (object->bIsCrouched)
            Object_ResetAnim(object, ANIM_CrouchPunch);
          else
            Object_ResetAnim(object, ANIM_StandPunch);
          object->hitState++;

          printf("** Hit Begin\n");
        }
        else if (object->hitState == 1)
        {
          if (Animation_IsEnded(object->frameCurrent, object->frameTicks, object->frameAnimation))
          {
            object->bIsHitting = false;
            object->hitState = 0;

            printf("** Hit End\n");

            if (object->bIsCrouched)
              Object_ResetAnimEnd(object, ANIM_CrouchDown);
            else
              Object_ResetAnimEnd(object, ANIM_Stand);
          }
        }
      }
    }

    MoveFlags2Acceleration(object);
  
  }

  velocityX = SolveVelocity(velocityX, object->accelerationX, 50, 400);
  object->velocityX = velocityX;
  object->x += object->velocityX;

  velocityY = SolveVelocity(velocityY, object->accelerationY, 50, 400);
  object->velocityY = velocityY;
  object->y += object->velocityY;
  object->y = ClampPosition(object->y, &object->velocityY, 0, 6400);
  velocityY = object->velocityY;

  if (!object->bIsDead && object->bIsBeingDamaged == false)
  {
    if (velocityX > 0)
      object->bDirection = 1;
    else if (velocityX < 0)
      object->bDirection = 0;
  }

  object->moveFlags = 0;

  object->sx = object->x / SCALE;  // (For now doesn't include screen scrolling, clipping, etc.)
  object->sy = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->y / SCALE);

  object->bounds.x0 = object->x + CHARACTER_FRAME_W * 50 - CHARACTER_FRAME_W * 25;
  object->bounds.y0 = object->y;
  object->bounds.x1 = object->x + CHARACTER_FRAME_W * 50 + CHARACTER_FRAME_W * 25;
  object->bounds.y1 = object->y + CHARACTER_FRAME_H * 100;

  if (object->bDirection == 1)
  {
    object->boundsHit.x0 = object->bounds.x1;
    object->boundsHit.x1 = object->bounds.x1 + 100 * 16;
    object->boundsHit.y0 = object->bounds.y0 + 100 * 28;
    object->boundsHit.y1 = object->bounds.y0 + 100 * 29;
  }
  else
  {
    object->boundsHit.x0 = object->bounds.x0 - 100 * 16;
    object->boundsHit.x1 = object->bounds.x0;
    object->boundsHit.y0 = object->bounds.y0 + 100 * 28;
    object->boundsHit.y1 = object->bounds.y0 + 100 * 29;
  }

  if (!object->bIsDead && 
      object->bIsHitting && object->hitState == 1)
  {
    for(u16 i=0;i < MAX_OBJECTS;i++)
    {
      Object* other = &sObjects[i];
      if (other == object)
        continue;
      if (other->type == OT_None)
        continue;
      if (other->bIsDead)
        continue;

      i32 x1 = 0;
      
      if (object->bDirection == 1)
        x1 = object->boundsHit.x1;
      else
        x1 = object->boundsHit.x0;

      i32 y0 = object->boundsHit.y0;
      i32 y1 = object->boundsHit.y1;

      if (Collision_BoxVsBox_Simple(&object->boundsHit, &other->bounds))
      {
        object->hitState = 2;

        if (other->bIsBeingDamaged == false)
        {

          other->bAiStayDistance = 0;

          other->damageTimer = 8;
          other->bIsBeingDamaged = 1;
          if (object->bDirection == 1)
            other->bDirection = 0;
          else
            other->bDirection = 1;
          
          if (other->hp > 0)
          {
            int amount = 1;

            if (other->bIsDazed)
            {
              amount = 2;
            }

            i32 hp = (i32)(other->hp) - amount;
            if (hp < 0)
              hp = 0;
            other->hp = hp;

          }
          Object_ResetAnim(other, ANIM_StandHit);
          printf("** Damage Begin\n");
          break;
        }
      }
    }
  }

  if (object->bIsDead     == false &&
      object->bIsBlocking == false && 
      object->bIsCrouched == false && 
      object->bIsHitting  == false &&
      object->bIsBeingDamaged == false)
  {
    if (!wasMoving && IsMoving(object))
    {
      Object_ResetAnim(object, ANIM_Walk);
      printf("** Walk\n");
    }
    else if (wasMoving && !IsMoving(object))
    {
      Object_ResetAnim(object, ANIM_Stand);
      printf("** Stand\n");
    }
  }

  u8 ended = 0;
  Animation_NextFrame(&object->frameTicks, &object->frameCurrent, &ended, object->frameAnimation);
  object->bFrameAnimationEnded = ended;

  if (object->type == OT_Player)
  {
    Canvas_PrintF(0, 0, &FONT_NEOSANS, 3, "S %i T %i F %i E %i Cr %i Bl %i Ht %i", object->moveState, object->frameTicks, object->frameCurrent, !!object->bFrameAnimationEnded, object->bIsCrouched, object->bIsBlocking, object->bIsHitting);
  }
}

static void Object_PreTick(Object* object)
{
  if (object->type == OT_Enemy)
  {
//    MarkDepth(object->x, object->y);
  }
  else if (object->type == OT_Player)
  {
    object->bIsCrouched = 0;
    object->bIsBlocking = 0;
  }
}

static void Object_SetPosition(Object* object, i32 x, u16 y)
{
  object->x = x;
  object->y = y;
}

static void Object_SetMoveDelta(Object* object, u8 moveVector)
{
  object->moveFlags = moveVector;
}

static void Object_SetMoveAction(Object* object, u8 moveAction)
{
  bool wasHitting = object->bIsHitting;

  object->bIsCrouched = ((moveAction & MA_Crouch) != 0);
  object->bIsBlocking = ((moveAction & MA_Block) != 0);
  object->bIsHitting  = ((moveAction & MA_Hit) != 0);

  if (object->bIsHitting)
  {
    object->bIsBlocking = 0;

    if (wasHitting)
    {
      object->hitTimer = 0;
      object->hitState = 0;
    }

  }

}

static void Object_Draw(Object* object)
{
  Draw_Animation(object->sx, object->sy - CHARACTER_FRAME_H, object->type, object->frameAnimation, object->frameCurrent, object->bDirection);

  if (object->bAiIsHead == 1)
  {
    Rect rect;
    rect.left   = object->sx;
    rect.top    = object->sy;
    rect.right  = rect.left + 2;
    rect.bottom = rect.top + 2;

    Canvas_DrawRectangle(16, rect);
  }

  #if 0
  Rect rect;
  rect.left   = object->bounds.x0 / 100;
  rect.top    = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->bounds.y0 / 100);
  rect.right  = object->bounds.x1 / 100;
  rect.bottom = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->bounds.y1 / 100);

  u8 colour = 16;

  if (object->bIsBeingDamaged)
  {
    colour = 17;
  }

  Canvas_DrawRectangle(colour, rect);

  rect.left   = object->boundsHit.x0 / 100;
  rect.top    = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->boundsHit.y0 / 100);
  rect.right  = object->boundsHit.x1 / 100;
  rect.bottom = SCREEN_HEIGHT - SCREEN_BOTTOM_EDGE - (object->boundsHit.y1 / 100);

  Canvas_DrawRectangle(26, rect);

  rect.left   = object->sx;
  rect.top    = object->sy;
  rect.right  = object->sx + (object->hp);
  rect.bottom = rect.top + 2;

  Canvas_DrawRectangle(12, rect);
  #endif
}

static void Object_Initialise(Object* object, u8 type)
{
  SDL_memset(object, 0, sizeof(Object));
  
  object->type = type;
  object->moveSpeedX = 100;
  object->moveSpeedY = 100;
  object->hp         = 4;
  object->aiHitTimer = 16;
  object->bAiStayDistance = 1;
  object->bAiIsHead  = 0;
}

static void Object_Clear(Object* object)
{
  object->type = OT_None;
}
