#include "functions.h"

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
