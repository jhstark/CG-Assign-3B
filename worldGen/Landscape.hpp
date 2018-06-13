// Jordan Hoskin-Stark
// Replaced by HeightMap
#ifndef _LANDSCAPE_HPP
#define _LANDSCAPE_HPP

#include "Object.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

class Landscape:public Object{
	
	public:
		Landscape(float size);
		
	private:
		// Functions
		void generateGround(std::string,float,glm::vec3);
		std::vector<float> findNormals(std::vector<float>);
};

#endif