#include "geometry.h"

bool Cjing3D::Rect::Intersects(const Rect& rect) const
{
	return Collision2D(GetPos(), GetSize(), rect.GetPos(), rect.GetSize());
}

bool Cjing3D::Rect::Intersects(const F32x2& point) const
{
	return Collision2D(GetPos(), GetSize(), point, {1.0f, 1.0f});
}
