#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "metaHeader.h"

class Camera
{

	glm::mat4 viewMat;
	glm::mat4 projectMat;
	glm::mat4 model;

	glm::vec3 fwd;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 pos;

	float movespeed;
	float rotspeed;

	float angleX;
	float angleY;

	int mouseX;
	int mouseY;

	float value = 0;
	glm::vec3 currcamPos;

public:
	Camera();

	glm::vec3 getPos();
	void onUpdate();
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::mat4 getMatrix();

};

#endif