#ifndef _WATER_HPP
#define _WATER_HPP

#include "Object.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

class Water:public Object{
	public:
		Water(float scale, float size);
    private:
        void generate(float size);
        float proceduralGeom(float x, float z);
        std::vector<float> findNormals(std::vector<float>);
        std::vector<float> generateTexCoords(std::vector<float>, float size);
        std::map<std::vector<float>, std::vector<glm::vec3> > nMap;
};

#endif
