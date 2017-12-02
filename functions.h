#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "data.h"

void Init(Settings* settings);
void Start();
void Step();

void Draw_Animation(i32 x, i32 y, u32 animation, u32 frame, i8 direction);
void Animation_GetInfo(u8 type, u8* speed, u8* frameCount, u8* animStyle);

void Objects_Setup();
void Objects_Teardown();
void Objects_Tick();
void Objects_Draw();

u16  Objects_Create(u8 type);
void Objects_Destroy(u16 id);

void Objects_SetMovementVector(u16 object, u8 movementVector);

#endif
