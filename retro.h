#ifndef RETRO_H
#define RETRO_H

#ifdef _WIN32
#define RETRO_WINDOWS
#endif

#ifdef __EMSCRIPTEN__
#define RETRO_BROWSER
#endif

#define Kilobytes(N) ((N) * 1024)
#define Megabytes(N) (Kilobytes(N) * 1024)

#ifndef RETRO_WINDOW_CAPTION
#define RETRO_WINDOW_CAPTION "Retro"
#endif

#ifndef RETRO_WINDOW_DEFAULT_WIDTH
#define RETRO_WINDOW_DEFAULT_WIDTH 640
#endif

#ifndef RETRO_WINDOW_DEFAULT_HEIGHT
#define RETRO_WINDOW_DEFAULT_HEIGHT 480
#endif

#ifndef RETRO_CANVAS_DEFAULT_WIDTH
#define RETRO_CANVAS_DEFAULT_WIDTH 320
#endif

#ifndef RETRO_CANVAS_DEFAULT_HEIGHT
#define RETRO_CANVAS_DEFAULT_HEIGHT 240
#endif

#ifndef RETRO_SOUND_DEFAULT_VOLUME
#define RETRO_SOUND_DEFAULT_VOLUME 128
#endif

#ifndef RETRO_FRAME_RATE
#define RETRO_FRAME_RATE 30
#endif

#ifndef RETRO_ARENA_SIZE
#define RETRO_ARENA_SIZE Kilobytes(1)
#endif

#ifndef RETRO_MAX_INPUT_ACTIONS
#define RETRO_MAX_INPUT_ACTIONS 32
#endif

#ifndef RETRO_MAX_INPUT_BINDINGS
#define RETRO_MAX_INPUT_BINDINGS 4
#endif

#ifndef RETRO_MAX_ANIMATED_SPRITE_FRAMES
#define RETRO_MAX_ANIMATED_SPRITE_FRAMES 8
#endif

#ifndef RETRO_CANVAS_COUNT
#define RETRO_CANVAS_COUNT 2
#endif

#ifndef RETRO_MAX_SOUND_OBJECTS
#define RETRO_MAX_SOUND_OBJECTS 16
#endif

#ifndef RETRO_AUDIO_FREQUENCY
#define RETRO_AUDIO_FREQUENCY 48000
#endif

#ifndef RETRO_AUDIO_CHANNELS
#define RETRO_AUDIO_CHANNELS 2
#endif

#ifndef RETRO_AUDIO_SAMPLES
#define RETRO_AUDIO_SAMPLES 16384
#endif 

#ifndef RETRO_TILE_SIZE
#define RETRO_TILE_SIZE 8
#endif

#ifndef RETRO_MAP_WIDTH
#define RETRO_MAP_WIDTH 40
#endif

#ifndef RETRO_MAP_HEIGHT
#define RETRO_MAP_HEIGHT 20
#endif

#define RETRO_UNUSED(X) (void)X
#define RETRO_ARRAY_COUNT(X) (sizeof(X) / sizeof((X)[0]))

#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;

#define Min(X, Y) (X < Y ? X : Y)
#define Max(X, Y) (X > Y ? X : Y)
#define Clamp(X, MIN, MAX) Max(Min(X, MIN), MAX)

typedef u8 SpriteHandle;
typedef u8 AnimationHandle;

typedef struct
{
  SDL_Texture*  texture;
  u8*    imageData;
  u16    w, h;
} Bitmap;

typedef struct
{
  i32 x, y;
} Point;

typedef struct
{
  i32 w, h;
} Size;

typedef struct
{
  i32 left, top, right, bottom;
} Rect;

typedef struct
{
  Bitmap*  bitmap;
  SDL_Rect rect;
  SpriteHandle spriteHandle;
} Sprite;

typedef struct
{
  Bitmap*         bitmap;
  u8              frameCount;
  u8              w, h;
  u16             frameLength;
  AnimationHandle animationHandle;
  SDL_Rect        frames[RETRO_MAX_ANIMATED_SPRITE_FRAMES];
} Animation;

typedef enum
{
  // Flip drawing X
  SOF_FlipX         = 1,
  // Flip drawing Y
  SOF_FlipY         = 2,
  // If AnimatedSpriteObject object then play animation
  SOF_Animation     = 4,
  // If AnimatedSpriteObject object then don't loop around the animation
  SOF_AnimationOnce = 8,
} SpriteObjectFlags;

typedef struct
{
  i32  x, y;
  i8   flags;
  u8   spriteHandle;
} StaticSpriteObject;

typedef struct
{
  i32        x, y;
  u8         w, h;
  u8         frameNumber, flags;
  u8         animationHandle;
  u16        frameTime;
} AnimatedSpriteObject;

typedef struct
{
  u8 r, g, b, a;
} Colour;

typedef struct
{
  Colour colours[256];
  u8     count, fallback, transparent;
} Palette;

typedef struct
{
  Bitmap bitmap;
  u8     height;
  u16    x[256];
  u8     widths[256];
} Font;

typedef struct
{
  u16     windowWidth;
  u16     windowHeight;
  u8      canvasWidth;
  u8      canvasHeight;
  f32     soundVolume;
  Palette palette;
} Settings;

typedef struct
{
  u32    start, paused;
  u8     flags;
} Timer;

typedef struct
{
  i32 length;
  u8* buffer;
  SDL_AudioSpec spec;
} Sound;

#define Point_Translate(P, X_VALUE, Y_VALUE) \
  (P)->x += X_VALUE; \
  (P)->y += Y_VALUE;

#define Point_Set(P, X_VALUE, Y_VALUE) \
  (P)->x = X_VALUE; \
  (P)->y = Y_VALUE;

#define Rect_Translate(R, X, Y) \
  (R)->left   += X; \
  (R)->top    += Y; \
  (R)->right  += X; \
  (R)->bottom += Y;

#define Rect_GetWidth(R) \
  ((R)->right - (R)->left)

#define Rect_GetHeight(R) \
  ((R)->right - (R)->left)

#define Colour_Equals(A, B) \
  (A.r == B.r && A.g == B.g && A.b == B.b)

Size Size_Make(u32 w, u32 h);

void Arena_Save(const char* filename);

void Arena_Load(const char* filename, bool loadMusic);

u8* Arena_SaveToMem(u32* size);

void Arena_LoadFromMem(u8* mem, bool loadMusic);

void Scope_Push(int name);

int  Scope_GetName();

u8* Scope_Obtain(u32 size);

#define Scope_New(T) ((T*) Scope_Obtain(sizeof(T)))

void Scope_Rewind();

void Scope_Pop();

Colour Colour_Make(u8 r, u8 g, u8 b);

void* Resource_Load(const char* name, u32* outSize);

char* TextFile_Load(const char* name, u32* outSize);

// Loads a bitmap and matches the palette to the canvas palette best it can.
void  Bitmap_Load(const char* name, Bitmap* outBitmap, u8 transparentIndex);

// Loads a bitmap with the palette order matching exactly the canvas palette.
// Offset is given to offset the loaded order by N colours
void  Bitmap_LoadPaletted(const char* name, Bitmap* outBitmap, u8 colourOffset);

// Loads a bitmap and matches the palette to the canvas palette best it can.
void  Bitmap_Load24(const char* name, Bitmap* outBitmap, u8 transparentR, u8 transparentG, u8 transparentB);

// Loads a bitmap, and swaps the palette with the given one.
void  Bitmap_Load24_PaletteSwap(const char* name, Bitmap* outBitmap, u8 transparentR, u8 transparentG, u8 transparentB,  Palette* src, Palette* dst);

void  Sprite_Make(Sprite* inSprite, Bitmap* bitmap, u32 x, u32 y, u32 w, u32 h);

Sprite* SpriteHandle_Get(SpriteHandle id);

void  Animation_LoadHorizontal(Animation* inAnimatedSprite, Bitmap* bitmap, u8 numFrames, u8 frameLengthMilliseconds, u32 originX, u32 originY, u32 frameWidth, u32 frameHeight);

void  Animation_LoadVertical(Animation* inAnimatedSprite, Bitmap* bitmap, u8 numFrames, u8 frameLengthMilliseconds, u32 originX, u32 originY, u32 frameWidth, u32 frameHeight);

Animation* AnimationHandle_Get(AnimationHandle id);

Size  Screen_GetSize();

i32   Canvas_GetWidth();

i32   Canvas_GetHeight();

void  Canvas_Set(u8 id);

typedef enum
{
  // Clear each frame
  CNF_Clear      = 1,
  // Apply blending to the canvas underneath
  CNF_Blend      = 2,
  // Render this canvas
  CNF_Render     = 4,
} CanvasFlags;

void  Canvas_SetFlags(u8 id, u8 flags, u8 clearColour);

void  Canvas_Splat(Bitmap* bitmap, i32 x, i32 y, Rect* srcRectangle);

void  Canvas_Splat2(Bitmap* bitmap, i32 x, i32 y, SDL_Rect* srcRectangle);

void  Canvas_Splat3(Bitmap* bitmap, SDL_Rect* dstRectangle, SDL_Rect* srcRectangle);

typedef enum
{
  FF_None     = SDL_FLIP_NONE,
  FF_FlipHorz = SDL_FLIP_HORIZONTAL,
  FF_FlipVert = SDL_FLIP_VERTICAL,
  FF_FlipDiag = FF_FlipHorz | FF_FlipVert,
  FF_Mask     = FF_FlipHorz | FF_FlipVert
} FlipFlags;

void  Canvas_SplatFlip(Bitmap* bitmap, SDL_Rect* dstRectangle, SDL_Rect* srcRectangle, u8 flipFlags);

void  Canvas_Place(StaticSpriteObject* spriteObject);

void  Canvas_Place2(Sprite* sprite, i32 x, i32 y);

void  Canvas_PlaceAnimated(AnimatedSpriteObject* spriteObject, bool updateTiming);

void  Canvas_PlaceAnimated2(Animation* animatedSprite, i32 x, i32 y, u8 frame, u8 flipFlags);

void  Canvas_PlaceScaled(Sprite* sprite, u32 x, u32 y, u32 scale);

void  Canvas_PlaceScaledF(Sprite* sprite, u32 x, u32 y, float scale);

void  Canvas_Flip();

void  Canvas_Clear();

void  Canvas_DrawPalette(Palette* palette, u32 Y);

void  Canvas_DrawRectangle(u8 Colour, Rect rect);

void  Canvas_DrawFilledRectangle(u8 Colour, Rect rect);

void  Canvas_PrintF(u32 x, u32 y, Font* font, u8 colour, const char* fmt, ...);

i32  Canvas_LengthF(Font* font, const char* fmt, ...);

typedef enum
{
  // 1
  FP_Normal,
  //  alpha + (k * beta)
  FP_WaveH,
  //  alpha + (k * beta)
  FP_WaveV,
  //  w *= alpha, h *= beta, x -= w/2, y -= h/2
  FP_Scale
} FramePresentation;

void  Canvas_SetPresentation(FramePresentation presentation, float alpha, float beta);

void  AnimatedSpriteObject_Make(AnimatedSpriteObject* inAnimatedSpriteObject, Animation* animation, i32 x, i32 y);

void  AnimatedSpriteObject_PlayAnimation(AnimatedSpriteObject* animatedSpriteObject, bool playing, bool loop);

void  AnimatedSpriteObject_SwitchAnimation(AnimatedSpriteObject* animatedSpriteObject, Animation* newAnimation, bool animate);

void  Sound_Load(Sound* sound, const char* name);

void  Sound_Play(Sound* sound, u8 volume);

void  Sound_Clear();

void  Music_Play(const char* name);

void  Music_Stop();

void  Palette_Make(Palette* palette);

void  Palette_SetDefault(const Palette* palette);

void  Palette_LoadFromBitmap(const char* name, Palette* palette);

void  Palette_Add(Palette* palette, Colour colour);

void  Palette_AddARGBInt(Palette* palette, u32 argb);

u8    Palette_FindColour(Palette* palette, Colour colour);

bool  Palette_HasColour(Palette* palette, Colour colour);

void  Palette_CopyTo(const Palette* src, Palette* dst);

#define Palette_GetColour(PALETTE, INDEX) \
  ((PALETTE)->colours[INDEX >= (PALETTE)->count ? (PALETTE)->fallback : INDEX])

void  Font_Make(Font* font);

void  Font_Load(const char* name, Font* font, Colour markerColour, Colour transparentColour);

int   Input_TextInput(char* str, u32 capacity);

void  Input_BindKey(int key, int action);

void  Input_BindAxis(int axis, int action);

bool  Input_GetActionDown(int action);

bool  Input_GetActionReleased(int action);

bool  Input_GetActionPressed(int action);

i16   Input_GetActionNowAxis(int action);

i16   Input_GetActionDeltaAxis(int action);

Point Input_MousePosition();

bool Input_MouseReleased();

bool Input_MousePressed();

bool Input_MouseDown();

bool Input_MouseHeldDown();

void  Timer_Make(Timer* timer);


void  Timer_Start(Timer* timer);

void  Timer_Stop(Timer* timer);

void  Timer_Pause(Timer* timer);

void  Timer_Unpause(Timer* timer);

u32   Timer_GetTicks(Timer* timer);

bool  Timer_IsStarted(Timer* timer);

bool  Timer_IsPaused(Timer* timer);

void  Init(Settings* s);

void  Start();

void  Step();

int   Random_Range(int min, int max);

int   Random_Roll(int Dice);

int   Random_Roll2(int Dice, int EqualsOrHigher);

void  Tile_Draw(Bitmap* bitmap, i32 x, i32 y, u32 tileX, u32 tileY);

void  Tile_Draw2(Bitmap* bitmap, i32 x, i32 y, u32 tileX, u32 tileY, u32 tileSize);

char* Text_SkipWhitespace(char* s);

char* Text_SkipToDigit(char* s);

char* Text_SkipString(char* s, const char* str);

bool Text_SkipToString(char* s, char** t, const char* str);

char* Text_ReadUInt(char* s, u32* i);

i32  Map_XYToIndex(u32 x, u32 y);

i32  Map_XYToIndex2(u32 x, u32 y, u32 mapWidth, u32 mapHeight);

bool Map_MouseToMapXY(i32* x, i32* y, i32 mapScreenOriginX, i32 mapScreenOriginY);

bool Map_MouseToMapXY2(i32* x, i32* y, i32 mapScreenOriginX, i32 mapScreenOriginY, u32 mapWidth, u32 mapHeight, u32 tileSize);

#endif
