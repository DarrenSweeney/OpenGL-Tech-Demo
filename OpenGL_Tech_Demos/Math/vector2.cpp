#include "vector2.h"

vector2::vector2()
	: x(0), y(0) {}

vector2::vector2(float _x, float _y)
	: x(_x), y(_y) {}

vector2 vector2::operator-(vector2 &vec2)
{
	return vector2(x - vec2.x, y - vec2.y);
}

vector2 vector2::operator-=(vector2 &vec2)
{
	x -= vec2.x;
	y -= vec2.y;

	return *this;
}