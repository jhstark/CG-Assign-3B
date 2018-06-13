#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(){
	initEye = glm::vec3(0.0f , 0.1f , -2.5f);
	at = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	reset();
}

void Camera::reset(){
	// Quick camera for now
	eye = initEye;
	viewMtx = glm::lookAt(eye, at, up);
}

glm::mat4 Camera::getView(){
	return viewMtx;
}

void Camera::lookAt(glm::vec3 pos){
	
	glm::vec3 at(pos.x, pos.y, pos.z);
	viewMtx = glm::lookAt(initEye, at, up);
	
}

void Camera::switchCam(){
	
	if (type == "fixed"){
		type = "follow";
	}
	else{
		type = "fixed";
	}
	std::cout << "Camera set to: " << type << std::endl;
}

void Camera::followPlane(glm::vec3 pos){
	
	glm::vec3 at(pos.x, pos.y, pos.z);
	eye.y = pos.y + 1.0;
	eye.z = pos.z + 2.0;
	camPos = initEye;
	viewMtx = glm::lookAt(initEye, at, up);
	
}

void Camera::update(float scale , glm::vec3 pos,glm::vec3 dir){
	if (type == "fixed"){
		lookAt(pos);
	}
	else{
		float scaleFact = 50 * scale;
		glm::vec3 at(pos.x, pos.y, pos.z);
		
		float yOffset = scaleFact*sin(dir.y);
	
		glm::vec3 from(pos.x - scaleFact * sin(dir.z), (pos.y + scaleFact/3) + yOffset , pos.z - scaleFact * cos(dir.z));
		camPos = from;
		viewMtx = glm::lookAt(from, at, up);
	}
	//viewMtx = glm::lookAt(eye, at, up);
}
	