#ifndef _HEIGHTMAP_HPP
#define _HEIGHTMAP_HPP

#include "Object.hpp"
#include "../libraries/stb_image.h"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

class HeightMap:public Object{
	public:
	    HeightMap(float , std::string, float size);
        std::vector<std::vector<int> > heightMap;
    private:
        void loadHeightMap(std::string);
        std::vector<float> findNormals(std::vector<float>);
        std::vector<float> generateTexCoords(std::vector<float>, float size);
        std::map<std::vector<float>, std::vector<glm::vec3> > nMap;
        void generate(float size);
        int height;
        int width;
};

#endif
