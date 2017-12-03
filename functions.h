#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "data.h"
#include <stdio.h>
#include <stdlib.h>

void Init(Settings* settings);
void Start();
void Step();

void Draw_Animation(i32 x, i32 y, u8 type, u32 animation, u32 frame, i8 direction);

u8   Animation_FirstFrame(u8 animation);
u8   Animation_LastFrame(u8 animation);
u8   Animation_Speed(u8 animation);
bool Animation_IsEnded(u8 frame, u8 ticks, u8 animation);
void Animation_NextFrame(u8* ticks, u8* frame, u8* ended, u8 animation);

bool Collision_BoxVsBox_Simple(Hitbox* self, Hitbox* other);
bool Collision_BoxVsBox(HitboxResult* outHit, Hitbox* self, Hitbox* other);

void Objects_Setup();
void Objects_Teardown();
void Objects_PreTick();
void Objects_Tick();
void Objects_Draw();

u16  Objects_Create(u8 type);
void Objects_Destroy(u16 id);

void Objects_SetTrackingObject(u16 id, u16 other);
void Objects_SetPosition(u16 object, i32 x, u16 depth);
void Objects_SetMovementVector(u16 object, u8 movementVector);
void Objects_SetMovementAction(u16 object, u8 movementAction);

#endif
