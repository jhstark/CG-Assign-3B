// Jordan Hoskin-Stark
#ifndef _PLANE_HPP
#define _PLANE_HPP

#include "Object.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

class Plane:public Object{
	
	public:
		Plane(float);		// Sets initial values
		void updatePos(std::map< std::string , bool > , double);
		void updateVelocity(float);
		void resetPos(double);
		glm::mat4 getModelMat();
		glm::vec3 direction;
		
	private:
		float v;			// velocity unit per second
		float h;			// height units
		glm::vec3 lastRpy = glm::vec3(0.0); // Roll pitch yaw (radians)
		glm::vec3 lastPos = glm::vec3(0.0);
		double lastUpdate;
		// Functions
		
};

#endif