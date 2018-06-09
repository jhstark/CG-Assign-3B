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
		void resetPos(double);
		glm::mat4 getModelMat();
		
	private:
		float v;			// velocity unit per second
		float h;			// height units
		glm::vec3 lastRpy = glm::vec3(0.0); // Roll pitch yaw (radians)
		glm::mat4 posMat;
		void updatePosMat(bool, bool);
		double lastUpdate;
		// Functions
		
};

#endif