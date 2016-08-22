#ifndef MATRIX3X4_H
#define MATRIX3X4_H

#include "vector3.h"
#include "vector4.h"
#include "MathHelper.h"
#include <math.h>

/*
	TODO: Take out binary operators + because copy of object. (Decide this.)
	TODO: Return a reference?
*/

// This is a column-major 4x4 matrix.
class Matrix4
{
public:
	float data[16];

	Matrix4();
	Matrix4(int m11, int m12, int m13, int m14,
		int m21, int m22, int m23, int m24,
		int m31, int m32, int m33, int m34,
		int m41, int m42, int m43, int m44);

	vector4 operator*(vector4 &vec4);
	Matrix4 operator+=(float value);
	Matrix4 operator-=(float value);
	Matrix4 operator*=(float value);
	Matrix4 operator*=(Matrix4 &matrix);
	Matrix4 operator=(Matrix4 &matrix);

	Matrix4 &scale(vector3 &vec3);
	Matrix4 &translate(vector3 &vec3);

	Matrix4 &rotate(float angle, vector3 &vec3);
	Matrix4 &rotateX(float angle);
	Matrix4 &rotateY(float angle);
	Matrix4 &rotateZ(float angle);

	Matrix4 &setFrustrum(float l, float r, float b, float t, float n, float f);
	Matrix4 &perspectiveProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
	Matrix4 &orthographicProjection(float left, float right, float bottom, float top, float near, float far);
	Matrix4 &lookAt(vector3 &position, vector3 &target, vector3 &worldUp);
	Matrix4 &transpose(Matrix4 matrix);
};

Matrix4 operator*(Matrix4 &matrixA, Matrix4 &matrixB);

#endif