#include "camera.h"

Camera::Camera(vector3 &_position, vector3 &worldUp, GLfloat _yaw, GLfloat _pitch, GLfloat speed, GLfloat sensitivity, GLfloat zoom)
	: movementSpeed(speed), mouseSensitivity(sensitivity), zoom(zoom), cameraSpeed(60.0f), ampletude(0.036f), frequincy(0.077f)
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

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix2()
{
	return glm::lookAt(glm::vec3(position.x, position.y, position.z),
		glm::vec3(position.x, position.y, position.z) + glm::vec3(frontVec.x, frontVec.y, frontVec.z), glm::vec3(upVec.x, upVec.y, upVec.z));
}

const GLfloat PI = 3.141592;
float camHeadBob = 0;
const float Speed = 8.0f;
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

#if 0
	if (camHeadBob > 4 * PI)
	{
		camHeadBob -= 4 * PI;
	}

	if (moving)
	{
		camHeadBob += frequincy;
	}
	else
		camHeadBob = 0.0f;

	position.y = 0.6f + (ampletude * cos(camHeadBob)) / 2.0f;
#endif

	UpdateCameraVectors();
}

void Camera::ControllerMovement()
{
	bool moving = false;
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

		if (axis[4] > 0.3)	// R2
			zoom -= 0.01f;
		if (axis[5] > 0.3)	// L2
			zoom += 0.01f;
	}

#if 0
	if (camHeadBob > 4 * PI)
	{ 
		camHeadBob -= 4 * PI; 
	}

	if (moving)
	{
		camHeadBob += frequincy;
	}
	else
		camHeadBob = 0.0f;

	position.y = 0.6f + (ampletude * cos(camHeadBob)) / 2.0f;
#endif

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