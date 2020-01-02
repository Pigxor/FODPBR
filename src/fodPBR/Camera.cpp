#include "Camera.h"

Camera::Camera()
{
	angleX = 0;
	angleY = 0;
	pos = glm::vec3(0, 0, 7);
	movespeed = 0.3f;
	rotspeed = 2.0f;
	//U-D Movement
	glm::mat4 t(1.0f);
	t = glm::translate(t, glm::vec3(0, 1,0));
	up = t * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	up = glm::normalize(up);
}

glm::vec3 Camera::getPos()
{
	return pos;
}

void Camera::onUpdate()
{
	//F-B Movement
	glm::mat4 t(1.0f);
	t = glm::rotate(t, glm::radians(angleX), glm::vec3(0, 1, 0));
	t = glm::translate(t, glm::vec3(0, 0, -1));
	fwd = t * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	fwd = glm::normalize(fwd);
	//L-R Movement
	t = glm::mat4(1.0f);
	t = glm::rotate(t, glm::radians(angleX), glm::vec3(0, 1, 0));
	t = glm::translate(t, glm::vec3(1, 0, 0));
	right = t * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	right = glm::normalize(right);
	//Rotation - Mouse Based
	const Uint32 mouse = SDL_GetMouseState(&mouseX, &mouseY);
	if (mouseX > (windowWidth / 2))
	{
		angleX -= rotspeed;
	}
	if (mouseX < (windowWidth / 2))
	{
		angleX += rotspeed;
	}
	if (mouseY > (windowHeight / 2))
	{
		angleY -= rotspeed * 0.8f;
	}
	if (mouseY < (windowHeight / 2))
	{
		angleY += rotspeed* 0.8f;
	}
	std::cout << angleX << " " << angleY << std::endl;
	//Keyboard Input For Movement
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_D])
	{
		pos += (right*movespeed);
	}
	if (state[SDL_SCANCODE_A])
	{
		pos -= (right*movespeed);
	}
	if (state[SDL_SCANCODE_W])
	{
		pos += (fwd*movespeed);
	}
	if (state[SDL_SCANCODE_S])
	{
		pos -= (fwd*movespeed);
	}
	if (state[SDL_SCANCODE_SPACE])
	{
		pos += (up*movespeed);
	}
	if (state[SDL_SCANCODE_LSHIFT])
	{
		pos -= (up*movespeed);
	}
	std::cout << getPos().x << " " << getPos().y << " " << getPos().x << std::endl;
	//Update Model Matrix
	model = getMatrix();
}

glm::mat4 Camera::getView()
{
	return viewMat;
}

glm::mat4 Camera::getProjection()
{
	projectMat = glm::perspective(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.f);
	return projectMat;
}

glm::mat4 Camera::getMatrix()
{
	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(angleX), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(angleY), glm::vec3(1, 0, 0));
	model = glm::translate(model, -pos);
	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1, 0, 0));
	viewMat = glm::inverse(model);
	return model;
}

