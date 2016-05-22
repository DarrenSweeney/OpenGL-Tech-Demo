#ifndef VECTOR_2_H
#define VECTOR_2_H

class vector2
{
public:
	float x, y;

	vector2();
	vector2(float x, float y);

	vector2 operator-(vector2 &vec2);
	vector2 operator-=(vector2 &vec2);
};

#endif