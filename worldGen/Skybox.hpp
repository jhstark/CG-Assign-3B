#ifndef _SKYBOX_HPP
#define _SKYBOX_HPP

#include "Object.hpp"

#include <vector>

class Skybox:public Object{
	
	public:
		Skybox();
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
		/* std::vector<std::string> faces{
			"models/sor_lake1/lake1_rt.png",
			"models/sor_lake1/lake1_lf.png",
			"models/sor_lake1/lake1_up.png",
			"models/sor_lake1/lake1_dn.png",
			"models/sor_lake1/lake1_ft.png",
			"models/sor_lake1/lake1_bk.png"
		}; */
		std::vector<std::string> faces{
			"models/skybox/right.jpg",
			"models/skybox/left.jpg",
			"models/skybox/top.jpg",
			"models/skybox/bottom.jpg",
			"models/skybox/front.jpg",
			"models/skybox/back.jpg",
		};
		
};

#endif