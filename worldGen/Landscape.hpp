#ifndef _LANDSCAPE_HPP
#define _LANDSCAPE_HPP

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

#include "Object.hpp"

class Landscape:public Object{
	
	public:
		Landscape(int size);
		
	private:
		// Functions
		void generateVertices(int);
		std::vector<float> findNormals(std::vector<float>);
};

#endif