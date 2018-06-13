#include "Plane.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD(x) ((x)*M_PI/180.0) 

Plane::Plane(float scale) : Object(scale){
	
	v = 0.0;
	h = 5.0;
	
	resetPos(0.0);
}

void Plane::resetPos(double dt){
	
	rpy = glm::vec3( 0.0 , 0.0 , DEG2RAD(180) );	// Roll pitch yaw (radians)
	pos = glm::vec3(0.0);	// x y z position
	pos.y = 1.0;
	
	lastUpdate = dt;
	
}

void Plane::updateVelocity(float new_v){
	v = new_v;
}

void Plane::updatePos(std::map< std::string , bool > keyPress, double timeOffset){
	
	double dt = timeOffset - lastUpdate;
	bool xRot,yRot,zRot = false; 
	lastRpy = rpy;
	lastPos = pos;
	float vTemp = v + abs(1/rpy.x);
	
	if (vTemp == 0){
		vTemp = 1;
	}
	
	float rotSpeed  = 5.0 / vTemp;
	
	// Change yaw on left and right arrows
	if (keyPress["left"] == true){
		zRot = true;
		rpy.z = rpy.z + rotSpeed * dt;
		rpy.x = std::max(rpy.x - 0.05,-0.8) ;
	}
	
	if (keyPress["right"] == true){
		zRot = true;
		rpy.z = rpy.z - rotSpeed * dt;
		rpy.x = std::min(rpy.x + 0.05,0.8);
	}
	
	if (keyPress["right"] == false && keyPress["left"] == false){
		if (rpy.x > 0.0){
			rpy.x = std::max(rpy.x - 0.1,0.0) ;
		}
		else if (rpy.x < 0.0){
			rpy.x = std::min(rpy.x + 0.1,0.0) ;
		}
	}
	
	
	// Change pitch on up and down arrows
	if (keyPress["up"] == true){
		xRot = true;
		rpy.y = std::max(rpy.y - dt, -1.3);
	}
	if (keyPress["down"] == true){
		xRot = true;
		rpy.y = std::min(rpy.y + dt, 1.3);
	}
	
	float tan_y = std::min(tan(rpy.y), 10.0);
	
	pos.x += (v * sin(rpy.z))/250;
	pos.z += (v * cos(rpy.z))/250;	
	pos.y -= (v * tan_y)/250;
	
	lastUpdate = timeOffset;
}
