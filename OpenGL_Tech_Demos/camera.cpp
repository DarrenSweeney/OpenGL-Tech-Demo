#include "camera.h"

Camera::Camera(vector3 &_position, vector3 &worldUp, GLfloat _yaw, GLfloat _pitch, GLfloat speed, GLfloat sensitivity, GLfloat zoom)
	: movementSpeed(speed), mouseSensitivity(sensitivity), zoom(zoom), cameraSpeed(60.0f)
{
	position = _position;
	upVec = worldUp;
	yaw = _yaw;
	pitch = _pitch;
	frontVec = vector3(0.0f, 0.0f, -1.0f);
	UpdateCameraVectors();
	movementSpeed = 15.0f;
}

Matrix4 &Camera::Camera::GetViewMatrix()
{
	return view.lookAt(position, position + frontVec, upVec);
}

void Camera::KeyboardMovement(bool keys[], GLfloat deltaTime)
{
	bool moving = false;

	GLfloat velocity = (movementSpeed * 1.0f) * deltaTime;
	if (keys[GLFW_KEY_W])
	{
		position += (frontVec * velocity);
		moving = true;
	}
	if (keys[GLFW_KEY_S])
	{
		position -= (frontVec * velocity);
		moving = true;
	}
	if (keys[GLFW_KEY_A])
	{
		position -= (frontVec.vectorProduct(upVec).normalise() * velocity);
		moving = true;
	}
	if (keys[GLFW_KEY_D])
	{
		position += (frontVec.vectorProduct(upVec).normalise() * velocity);
		moving = true;
	}

	UpdateCameraVectors();
}

void Camera::ControllerMovement()
{
	bool moving = false;
	// PlayStation Controller
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	const float *axis = NULL;
	int count;
	if (1 == present)
	{
		axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

		// Movement - Left Stick
		if (axis[0] > 0.2 || axis[0] < -0.2)
		{
			moving = true;
			position += ((frontVec.vectorProduct(upVec)).normalise()  * (axis[0]) * movementSpeed) * deltaTime;
		}

		if (axis[1] > 0.2 || axis[1] < -0.2)
		{
			moving = true;
			position -= (frontVec * (axis[1]) * movementSpeed) * deltaTime;
		}

		// Camera - Right Stick.
		if (axis[2] > 0.3 || axis[2] < -0.3)
			yaw += (axis[2] * (cameraSpeed)) * deltaTime;
		if (axis[3] > 0.3 || axis[3] < -0.3)
			pitch -= (axis[3] * (cameraSpeed)) * deltaTime;
	}

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	UpdateCameraVectors();
}

void Camera::MouseMovement(GLfloat xOffset, GLfloat yOffset)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	UpdateCameraVectors();
}

void Camera::Roll(GLfloat angle)
{
	rightVec = (rightVec * cos(MathHelper::DegressToRadians(angle)) +
				upVec * sin(MathHelper::DegressToRadians(angle))).normalise();

	upVec = frontVec.vectorProduct(rightVec).normalise();

	upVec *= -1;
}

void Camera::UpdateCameraVectors()
{
	vector3 frontVec;
	frontVec.x = cos(MathHelper::DegressToRadians(yaw)) * cos(MathHelper::DegressToRadians(pitch));
	frontVec.y = sin(MathHelper::DegressToRadians(pitch));
	frontVec.z = sin(MathHelper::DegressToRadians(yaw)) * cos(MathHelper::DegressToRadians(pitch));
	frontVec.normalise();

	this->frontVec = frontVec;
}