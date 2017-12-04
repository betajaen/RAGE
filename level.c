#include "functions.h"

#define TILE_SIZE 16
#define SECTION_W 20
#define SECTION_H 14

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
  u16 tiles[SECTION_W * SECTION_H];
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

  printf("%i", sectionIdx);

}

void Level_Draw()
{
  Section* section = &sLevel.sections[sLevel.currentSection];

  SDL_Rect src, dst;
  src.w = TILE_SIZE;
  src.h = TILE_SIZE;
  dst.w = src.w;
  dst.h = src.h;

  for(u32 i=0;i < SECTION_W;i++)
  {
    for(u32 j=0;j < SECTION_H;j++)
    {
      u32 x = i * TILE_SIZE;
      u32 y = j * TILE_SIZE;
      u16 tile = section->tiles[i + (j * SECTION_W)];
      u16 tx = (tile % 32) * TILE_SIZE;
      u16 ty = (tile / 32) * TILE_SIZE;

      src.x = tx;
      src.y = ty;
      dst.x = x;
      dst.y = y;

      Canvas_Splat3(&SPRITESHEET, &dst, &src);
    }
  }
}
