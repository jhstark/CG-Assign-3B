#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(){
	initEye = glm::vec3(0.0f , 3.0f , 5.0f);
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

void Camera::update(std::map< std::string , bool > keyPress){
	if (keyPress["w"]){
		eye.z = eye.z-0.5;
	}
	if (keyPress["s"]){
		eye.z = eye.z+0.5;
	}
	if (keyPress["a"]){
		eye.x = eye.x-0.5;
	}
	if (keyPress["d"]){
		eye.x = eye.x+0.5;
	}
	viewMtx = glm::lookAt(eye, at, up);
}
	