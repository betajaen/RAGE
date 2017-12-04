#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void Init(Settings* settings);
void Start();
void Step();

void Draw_Animation(i32 x, i32 y, u8 type, u32 animation, u32 frame, i8 direction, u8 depth);

u8   Animation_FirstFrame(u8 animation);
u8   Animation_LastFrame(u8 animation);
u8   Animation_Speed(u8 animation);
bool Animation_IsEnded(u8 frame, u8 ticks, u8 animation);
void Animation_NextFrame(u8* ticks, u8* frame, u8* ended, u8 animation);

void Level_Load(const char* name);
void Level_Draw(i32 offset);

void Level_StartSection(u8 sectionIdx);
void Level_NextSection();

bool Collision_BoxVsBox_Simple(Hitbox* self, Hitbox* other);
bool Collision_BoxVsBox(HitboxResult* outHit, Hitbox* self, Hitbox* other);

void Objects_Setup();
void Objects_Teardown();
void Objects_PreTick();
void Objects_Tick(bool stillScreen);
void Objects_Draw(i32 xOffset);
void Objects_Clear();
void Objects_ClearExcept(u8 type);

u16  Objects_FindFirstOf(u8 type);
u16  Objects_Create(u8 type);
void Objects_Destroy(u16 id);

void Objects_SetTrackingObject(u16 id, u16 other);
void Objects_SetTrackingObjectType(u8 type, u16 other);
void Objects_SetPosition(u16 object, i32 x, u16 depth);
void Objects_ModPosition(u16 object);
void Objects_SetMovementVector(u16 object, u8 movementVector);
void Objects_SetMovementAction(u16 object, u8 movementAction);

#endif
