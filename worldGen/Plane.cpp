#include "Plane.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

Plane::Plane(){
	
	v = 1.0;
	h = 5.0;
	
	resetPos(0.0);
}

void Plane::resetPos(double dt){
	
	rpy = glm::vec3(0.0);	// Roll pitch yaw (radians)
	pos = glm::vec3(0.0);	// x y z position
	posMat = glm::mat4();
	pos.y = h;
	posMat[3][2] = h;
	
	lastUpdate = dt;
	
}

void Plane::updatePos(std::map< std::string , bool > keyPress, double timeOffset){
	
	double dt = timeOffset - lastUpdate;
	bool xRot,yRot,zRot = false; 
	lastRpy = rpy;
	if (keyPress["left"] == true){
		zRot = true;
		rpy.z = rpy.z - dt;
	}
	if (keyPress["right"] == true){
		zRot = true;
		rpy.z = rpy.z + dt;
	}
	if (keyPress["up"] == true){
		xRot = true;
		rpy.x = rpy.x + dt;
	}
	if (keyPress["down"] == true){
		yRot = true;
		rpy.x = rpy.x - dt;
	}
	updatePosMat(zRot,xRot);
	// pos.x = pos.x + v * (dt);
	// pos.z = pos.z + v * (dt);
	
	lastUpdate = timeOffset;
	
}

void Plane::updatePosMat(bool zRot, bool yRot){
	
	glm::vec3 posX( posMat[0][0], posMat[1][0], posMat[2][0] );
	glm::vec3 posY( posMat[0][1], posMat[1][1], posMat[2][1] );
	glm::vec3 posZ( posMat[0][2], posMat[1][2], posMat[2][2] );

	float speed = 3.0;
	
	//Rotate around the models z axis (roll)
	if (zRot){
		
		float cosZ = cos(abs(speed*rpy.z - lastRpy.z));
		float sinZ = sin(abs(speed*rpy.z - lastRpy.z));
		glm::vec3 tmpX = posX;
		
		posX = glm::vec3(cosZ * posMat[0][0] , -sinZ*posMat[1][0] , posMat[2][0]);
		posY = glm::vec3(sinZ * posMat[0][1] , cosZ * posMat[1][1] , posMat[2][1]);
		
	}
	
	/* // Rotate about the model's y axis.
	if ( yRot )
	{
		float sinY = sin(lastRpy.x - rpy.x);
		float cosY = cos(lastRpy.x - rpy.x);

		glm::vec3 tmpX = posX;
		posX = cosY*tmpX + sinY*posZ;
		posZ = -sinY*tmpX + cosY*posZ;
	}
	// Rotate about the model's x axis.
	if ( xRot )
	{
		float sinX = sin(lastRpy.z - rpy.z);
		float cosX = cos(lastRpy.z - rpy.z);

		glm::vec3 tmpY = posY;
		posY = cosX*tmpY - sinX*posZ;
		posZ = sinX*tmpY + cosX*posZ;
	} */

	// Update the model matrix with new eye axes.
	posMat[0][0] = posX[0];
	posMat[1][0] = posX[1];
	posMat[2][0] = posX[2];
	
	posMat[0][1] = posY[0];
	posMat[1][1] = posY[1];
	posMat[2][1] = posY[2];
	
	posMat[0][2] = posZ[0];
	posMat[1][2] = posZ[1];
	posMat[2][2] = posZ[2];
}

glm::vec3 Plane::getPos(){
	return pos;
}
glm::mat4 Plane::getModelMat(){
	return posMat;
}

glm::vec3 Plane::getOrientation(){
	return rpy;
}
