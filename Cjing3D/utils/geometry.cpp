#include "geometry.h"

namespace Cjing3D {

	bool Rect::Intersects(const Rect& rect) const
	{
		return Collision2D(GetPos(), GetSize(), rect.GetPos(), rect.GetSize());
	}

	bool Rect::Intersects(const F32x2& point) const
	{
		return Collision2D(GetPos(), GetSize(), point, { 1.0f, 1.0f });
	}

	Rect& Rect::Offset(const F32x2& offset)
	{
		mLeft += offset[0];
		mTop += offset[1];
		mRight += offset[0];
		mBottom += offset[1];

		return *this;
	}

}