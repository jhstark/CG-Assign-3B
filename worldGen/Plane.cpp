#include "Plane.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

Plane::Plane(){
	
	v = 10.0;
	h = 5.0;
	
	resetPos(0.0d);
}

void Plane::resetPos(double dt){
	
	rpy = glm::vec3(0.0);	// Roll pitch yaw (radians)
	orientation = 0;		// radians, forward of craft
	pos = glm::vec3(0.0);	// x y z position
	
	lastUpdate = dt;
	
}

void Plane::updatePos(double timeOffset){
	
	double dt = timeOffset - lastUpdate;
	
	orientation = sin(timeOffset);
	
	//pos.x = pos.x + cos(orientation) * v * (dt);
	//pos.z = pos.z + sin(orientation) * v * (dt);
	
	lastUpdate = timeOffset;
	
}

glm::vec3 Plane::getPos(){
	return pos;
}

float Plane::getOrientation(){
	return orientation;
}