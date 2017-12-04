#include "functions.h"

#define TILE_SIZE 16
#define SECTION_W 20
#define SECTION_H 14
#define MAX_OBJECTS_PER_SECTION 16
#define SECTION_PX_W (SECTION_W * TILE_SIZE)

static char* skipWhitespace(char* s)
{
  while (*s != 0 && isspace(*s))
    s++;
  return s;
}

static char* skipToDigit(char* s)
{
  while (*s != 0 && !isdigit(*s))
    s++;
  return s;
}

static char* skipString(char* s, const char* str)
{
  s = skipWhitespace(s);
  s += strlen(str);
  s = skipWhitespace(s);
  return s;
}

static bool skipToString(char* s, char** t, const char* str)
{
  (*t) = strstr(s, str);
  if ((*t) == NULL)
    return false;
  return true;
}

static char* skipPassString(char* s, const char* str)
{
  s += strlen(str);
  return s;
}

static char* readUInt(char* s, u32* i)
{
  (*i) = 0;

  while (*s != 0 && isdigit(*s))
  {
    (*i) = (*i) * 10 + ((*s) - '0');
    s++;
  }

  return s;
}

typedef struct
{
  i32 x, y;
  u8  type;
  u8  flags;
} ObjectSpawn;

typedef struct
{
  u8          numObjects;
  u16         tiles[SECTION_W * SECTION_H];
  ObjectSpawn objects[MAX_OBJECTS_PER_SECTION];
} Section;

typedef struct
{
  u8       numSections;
  u8       currentSection;
  Section* sections;
} Level;

Level sLevel;

void Level_Load(const char* t)
{
  u32 dataSize;
  char* data = TextFile_Load(t, &dataSize);
  SDL_assert(data);
  
  skipToString(data, &data, "width=\"");
  SDL_assert(data);
  data = skipPassString(data, "width=\"");

  u32 width = 0;
  data = readUInt(data, &width);

  sLevel.numSections = width / SECTION_W;
  sLevel.currentSection = 0;
  sLevel.sections = malloc(sizeof(Section) * sLevel.numSections);
  SDL_memset(sLevel.sections, 0, sizeof(Section) * sLevel.numSections);

  skipToString(data, &data, "=\"csv\">");
  SDL_assert(data);
  data = skipPassString(data, "=\"csv\">");
  
  u32 sectionIdx = 0;
  u32 row = 0;
  u32 x = 0;
  u32 y = 0;
  for(u32 i=0;i < (width * SECTION_H);i++)
  {
    data = skipToDigit(data);
    u32 tileId;
    data = readUInt(data, &tileId);

    if (tileId > 0)
      tileId--;

    sLevel.sections[sectionIdx].tiles[x + (y * SECTION_W)] = tileId;

    row++;
    x++;
    if (x == SECTION_W)
    {
      x = 0;
      sectionIdx++;
    }

    if (row == width)
    {
      x = 0;
      row = 0;
      sectionIdx = 0;
      y++;
    }
  }

  while(true)
  {
    u32  type;
    u32  x, y;

    if (skipToString(data, &data, "gid=\"") == false)
      break;

    data = skipPassString(data, "gid=\"");
    data = readUInt(data, &type);

    skipToString(data, &data, "x=\"");
    SDL_assert(data);
    data = skipPassString(data, "x=\"");
    data = readUInt(data, &x);

    skipToString(data, &data, "y=\"");
    SDL_assert(data);
    data = skipPassString(data, "y=\"");
    data = readUInt(data, &y);
    
    u32 sectionIdx = x / SECTION_PX_W;
    x = x % SECTION_PX_W;
    
    y -= 144;

    y = 63 - y;

    if (y < 0)
      y = 0;
    else if (y > 63)
      y = 63;

    SDL_assert(sectionIdx < sLevel.numSections);

    Section* section = &sLevel.sections[sectionIdx];
    if (section->numObjects == MAX_OBJECTS_PER_SECTION)
    {
      printf("Out of objects room for section %i", sectionIdx);
      continue;
    }

    ObjectSpawn obj;
    obj.type = type - 1;
    obj.flags = 0;
    obj.x = (x + 8) * 100;
    obj.y = (y) * 100;

    section->objects[section->numObjects] = obj;
    section->numObjects++;

  }

}

static void DrawLevel(Section* section, i32 xOffset)
{

  SDL_Rect src, dst;
  // Tiles

  src.w = TILE_SIZE;
  src.h = TILE_SIZE;
  dst.w = src.w;
  dst.h = src.h;

  for (u32 i = 0; i < SECTION_W; i++)
  {
    for (u32 j = 0; j < SECTION_H; j++)
    {
      u32 x = i * TILE_SIZE;
      u32 y = j * TILE_SIZE;
      u16 tile = section->tiles[i + (j * SECTION_W)];
      u16 tx = (tile % 32) * TILE_SIZE;
      u16 ty = (tile / 32) * TILE_SIZE;

      src.x = tx;
      src.y = ty;
      dst.x = x + xOffset;
      dst.y = y;

      Canvas_Splat3(&SPRITESHEET, &dst, &src);
    }
  }
}

void Level_Draw(i32 offsetX)
{
  SDL_Rect src, dst;

  // Background Sky
  src.x = 416;
  src.y = 16;
  src.w = 80;
  src.h = 128;

  dst.y = 0;
  dst.w = src.w;
  dst.h = src.h;

  for (int i = 0; i < (320 / 80); i++)
  {
    dst.x = i * 80;
    Canvas_Splat3(&SPRITESHEET, &dst, &src);
  }

  if (offsetX != 0)
  {
    Section* sectionLast = &sLevel.sections[sLevel.currentSection - 1];
    DrawLevel(sectionLast, -offsetX);
    Section* section = &sLevel.sections[sLevel.currentSection];
    DrawLevel(section, 320 - offsetX);
  }
  else
  {
    Section* section = &sLevel.sections[sLevel.currentSection];
    DrawLevel(section, 0);
  }
}


void Level_StartSection(u8 sectionIdx)
{
  sLevel.currentSection = sectionIdx;
  Section* section = &sLevel.sections[sectionIdx];

  // Objects_ClearExcept(OT_Player);

  for(u32 i=0;i < section->numObjects;i++)
  {
    ObjectSpawn* spawn = &section->objects[i];
    u16 id = 0;
    switch(spawn->type)
    {
      case 3:
      {
        if (sectionIdx == 0)
        {
          if (Objects_FindFirstOf(OT_Player) == 0)
          {
            id = Objects_Create(OT_Player, 0xFF);
          }
        }
      }
      break;
      case 4:
      case 5:
      case 6:
      case 7:
      {
        id = Objects_Create(OT_Enemy, sectionIdx);
      }
      break;
      case 8:
      {
        // Passive
        // Objects_Create(OT_Passive);
      }
      break;
    }

    if (id == 0)
      continue;

    Objects_SetPosition(id, spawn->x, spawn->y);

  }

}

void Level_PrevSection()
{
  u8 prev = sLevel.currentSection - 1;

  if (prev == -1)
    Level_StartSection(0);
  else
    Level_StartSection(prev);
}

void Level_NextSection()
{
  u32 next = sLevel.currentSection + 1;
  if (next == sLevel.numSections)
    Level_StartSection(0);
  else
    Level_StartSection(next);
}

void Level_PostNextSection()
{
  if (sLevel.currentSection > 0)
  {
    Objects_DestroySection(sLevel.currentSection - 1);
  }
}
