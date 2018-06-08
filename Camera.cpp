#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(){
	initEye = glm::vec3(0.0f , 5.0f , 5.0f);
	at = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	reset();
}

void Camera::reset(){
	// Quick camera for now
	viewMtx = glm::lookAt(initEye, at, up);
}

glm::mat4 Camera::getView(){
	return viewMtx;
}

void Camera::lookAt(glm::vec3 pos){
	
	glm::vec3 at(pos.x, pos.y, pos.z);
	viewMtx = glm::lookAt(initEye, at, up);
	
}