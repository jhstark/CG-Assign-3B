// Jordan Hoskin-Stark
#ifndef _SKYBOX_HPP
#define _SKYBOX_HPP

#include "Object.hpp"

#include <vector>

class Skybox:public Object{
	
	public:
		Skybox(float);
		unsigned int textureID;
		void loadTexture();
		
	private:
		// Functions
		void generate();
		std::vector<float> findNormals(std::vector<float>);
		
			/* 
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg" */
		std::vector<std::string> faces{
			"models/skybox/right.tga",
			"models/skybox/left.tga",
			"models/skybox/top.tga",
			"models/skybox/bottom.tga",
			"models/skybox/front.tga",
			"models/skybox/back.tga",
		};
		
};

#endif