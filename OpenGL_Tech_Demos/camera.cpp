#include "camera.h"

Camera::Camera(vector3 &_position, vector3 &worldUp, GLfloat _yaw, GLfloat _pitch, GLfloat speed, GLfloat sensitivity, GLfloat zoom)
	: movementSpeed(speed), mouseSensitivity(sensitivity), zoom(zoom)
{
	position = _position;
	upVec = worldUp;
	yaw = _yaw;
	pitch = _pitch;
	frontVec = vector3(0.0f, 0.0f, -1.0f);
	UpdateCameraVectors();
}

Matrix4 &Camera::Camera::GetViewMatrix()
{
	return view.lookAt(position, position + frontVec, upVec);
}

const float Speed = 8.0f;
void Camera::KeyboardMovement(bool keys[], GLfloat deltaTime)
{
	GLfloat velocity = movementSpeed * deltaTime;
	if (keys[GLFW_KEY_W])
		position += (frontVec * velocity) * Speed;
	if (keys[GLFW_KEY_S])
		position -= (frontVec * velocity) * Speed;
	if (keys[GLFW_KEY_A])
		position -= (frontVec.vectorProduct(upVec).normalise() * velocity) * Speed;
	if (keys[GLFW_KEY_D])
		position += (frontVec.vectorProduct(upVec).normalise() * velocity) * Speed;

	UpdateCameraVectors();
}

const float SPEED = 5.3f;
void Camera::ControllerMovement()
{
	// PlayStation Controller
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	const float *axis = NULL;
	// std::cout << present << std::endl;
	int count;
	if (1 == present)
	{
		axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

		// Movement - Left Stick
		if (axis[0] > 0.2 || axis[0] < -0.2)
			position += (frontVec.vectorProduct(upVec)).normalise()  * (axis[0] / 50.0) * SPEED;
		if (axis[1] > 0.2 || axis[1] < -0.2)
			position -= frontVec * (axis[1] / 50.0) * SPEED;

		// Camera - Right Stick.
		if (axis[2] > 0.3 || axis[2] < -0.3)
			yaw += axis[2] * (SPEED / 15.0f);
		if (axis[3] > 0.3 || axis[3] < -0.3)
			pitch -= axis[3] * (SPEED / 15.0f);

		if (axis[4] > 0.3)	// R2
			zoom -= 0.01f;
		if (axis[5] > 0.3)	// L2
			zoom += 0.01f;
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

void Camera::MouseScroll(GLfloat yOffset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yOffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

const GLfloat PI = 3.141592;
GLfloat DegressToRadians(GLfloat degrees)
{
	return degrees * (PI / 180.0f);
}

void Camera::UpdateCameraVectors()
{
	vector3 frontVec;
	frontVec.x = cos(DegressToRadians(yaw)) * cos(DegressToRadians(pitch));
	frontVec.y = sin(DegressToRadians(pitch));
	frontVec.z = sin(DegressToRadians(yaw)) * cos(DegressToRadians(pitch));
	frontVec.normalise();

	this->frontVec = frontVec;
}