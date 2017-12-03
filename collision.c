#include "functions.h"

#define Sign(V) ((0 < (V)) - ((V) < 0))

static bool Intersection_BoxVsBox(HitboxResult* outHit, HitboxTest* self, HitboxTest* other)
{
//  assert(outHit);
//  assert(self);
//  assert(other);

  i32 dx, px, dy, py, s;

  dx = other->x - self->x;
  px = (other->halfWidth + self->halfWidth) - abs(dx);

  if (px <= 0)
    return false;

  dy = other->y - self->y;
  py = (other->halfHeight + self->halfHeight) - abs(dy);

  if (py <= 0)
    return false;

  if (px < py)
  {
    s = Sign(dx);
    outHit->delta.x = px * s;
    outHit->delta.y = 0;
    outHit->normal.x = s;
    outHit->normal.y = 0;
    outHit->position.x = self->x + (self->halfWidth * s);
    outHit->position.y = other->y;
  }
  else
  {
    s = Sign(dy);
    outHit->delta.x = 0;
    outHit->delta.y = py * s;
    outHit->normal.x = 0;
    outHit->normal.y = s;
    outHit->position.x = other->x;
    outHit->position.y = self->y + (self->halfHeight * s);
  }

  return true;
}

static bool intersectionOnly(HitboxTest* self, HitboxTest* other)
{
  i32 dx, px, dy, py;

  dx = other->x - self->x;
  px = (other->halfWidth + self->halfWidth) - abs(dx);

  if (px <= 0)
    return false;

  dy = other->y - self->y;
  py = (other->halfHeight + self->halfHeight) - abs(dy);

  if (py <= 0)
    return false;

  return true;
}

bool Collision_BoxVsBox_Simple(Hitbox* self, Hitbox* other)
{
  HitboxTest s;
  s.x = (self->x1 + self->x0) / 2;
  s.y = (self->y1 + self->y0) / 2;
  s.halfWidth = (self->x1 - self->x0) / 2;
  s.halfHeight = (self->y1 - self->y0) / 2;

  HitboxTest o;
  o.x = (other->x1 + other->x0) / 2;
  o.y = (other->y1 + other->y0) / 2;
  o.halfWidth = (other->x1 - other->x0) / 2;
  o.halfHeight = (other->y1 - other->y0) / 2;


  return intersectionOnly(&s, &o);

}

bool Collision_BoxVsBox(HitboxResult* outHit, Hitbox* self, Hitbox* other)
{
  HitboxTest s;
  s.x = (self->x1 + self->x0) / 2;
  s.y = (self->y1 + self->y0) / 2;
  s.halfWidth = (self->x1 - self->x0) / 2;
  s.halfHeight = (self->y1 - self->y0) / 2;

  HitboxTest o;
  o.x = (other->x1 + other->x0) / 2;
  o.y = (other->y1 + other->y0) / 2;
  o.halfWidth = (other->x1 - other->x0) / 2;
  o.halfHeight = (other->y1 - other->y0) / 2;

  return Intersection_BoxVsBox(outHit, &s, &o);
}