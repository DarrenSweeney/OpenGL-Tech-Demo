#include "MathHelper.h"

GLfloat MathHelper::DegressToRadians(GLfloat degrees)
{
	const GLfloat PI = 3.141592;

	return degrees * (PI / 180.0f);
}