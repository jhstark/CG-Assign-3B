#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera{
	
	public:
		Camera();		// Sets initial values
		void lookAt(glm::vec3);
		glm::mat4 getView();
		void reset();
		void update(glm::vec3,glm::vec3);
		void followPlane(glm::vec3);
		void switchCam();
		std::string type;
	private:
		glm::mat4 viewMtx;
		glm::vec3 initEye;
		glm::vec3 eye;
		glm::vec3 at;
		glm::vec3 up;
		
};

#endif