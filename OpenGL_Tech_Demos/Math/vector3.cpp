#include "vector3.h"
#include <math.h>

vector3::vector3()
	: x(0), y(0), z(0)
{
	
}

vector3::vector3(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z)
{
	
}

vector3 vector3::operator+(vector3 &vec3)
{
	return vector3(x + vec3.x, y + vec3.y, z + vec3.z);
}

vector3 vector3::operator+=(vector3 &vec3)
{
	x += vec3.x;
	y += vec3.y;
	z += vec3.z;

	return *this;
}

vector3 vector3::operator-(vector3 &vec3)
{
	return vector3(x - vec3.x, y - vec3.y, z - vec3.z);
}

vector3 vector3::operator-=(vector3 &vec3)
{
	x -= vec3.x;
	y -= vec3.y;
	z -= vec3.z;

	return *this;
}

vector3 vector3::operator*(vector3 &vec3)
{
	return vector3(x * vec3.x, y * vec3.y, z * vec3.z);
}

vector3 vector3::operator*(float value)
{
	return vector3(x * value, y * value, z * value);
}

vector3 vector3::operator*=(vector3 &vec3)
{
	x *= vec3.x;
	y *= vec3.y;
	z *= vec3.z;

	return *this;
}

vector3 vector3::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;

	return *this;
}

float vector3::dotProduct(vector3 &vec3)
{
	return x * vec3.x + y * vec3.y + z * vec3.z;
}

vector3 vector3::vectorProduct(vector3 &vec3)
{
	return vector3(y*vec3.z - z*vec3.y, z*vec3.x - x*vec3.z, x*vec3.y - y*vec3.x);
}

vector3 vector3::crossProduct(vector3 &vec3_One, vector3 &vec3_Two)
{
	return vector3(vec3_Two.y*vec3_One.z - vec3_Two.z*vec3_One.y, vec3_Two.z*vec3_One.x 
		- vec3_Two.x*vec3_One.z, vec3_Two.x*vec3_One.y - vec3_Two.y*vec3_One.x);
}

void vector3::makeOrthinormalBasis(vector3 &a, vector3 &b, vector3 &c)
{
	a.normalise();
	c = a.vectorProduct(b);

	if (c.squareMagnitude() == 0.0f)
		return;

	c.normalise();
	b = c.vectorProduct(a);
}

void vector3::clear()
{
	x, y, z = 0;
}

vector3 &vector3::invert()
{
	x *= -1;
	y *= -1;
	z *= -1;

	return *this;
}

float vector3::magnitude()
{
	return sqrtf(x*x + y*y + z*z);
}

float vector3::squareMagnitude()
{
	return (x*x + y*y + z*z);
}

vector3 vector3::normalise()
{
	if (magnitude() > 0)
	{
		*this *= 1 / magnitude();
	}

	return *this;
}