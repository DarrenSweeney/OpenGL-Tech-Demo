#ifndef VECTOR3_H
#define VECTOR3_H

class vector3
{
public:
	float x, y, z;
private:
	/*
		Commented out because GLSL wants a vec3 of stride 12 bytes.
	*/
	// float pad;

public:
	vector3();
	vector3(float x, float y, float z);

	vector3 operator+(vector3 &vec3);
	vector3 operator+=(vector3 &vec3);

	vector3 operator-(vector3 &vec3);
	vector3 operator-=(vector3 &vec3);

	vector3 operator*(vector3 &vec3);
	vector3 operator*(float value);
	vector3 operator*=(vector3 &vec3);
	vector3 operator*=(float value);

	float dotProduct(vector3 &vec3);
	vector3 vectorProduct(vector3 &vec3);
	vector3 crossProduct(vector3 &vec3_one, vector3 &vec3_two);

	void makeOrthinormalBasis(vector3 &a, vector3 &b, vector3 &c);

	void clear();
	vector3 &invert();
	float magnitude();
	float squareMagnitude();
	vector3 normalise();
};

#endif