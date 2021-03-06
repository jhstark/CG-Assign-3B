// Author: Jordan Hoskin-Stark
#ifndef _OBJECT_HPP
#define _OBJECT_HPP

#include "../libraries/stb_image.h"
#include "../libraries/tiny_obj_loader.h"

#include <iostream>
#include <vector>
#include <map>
#include "../libraries/openglH.h"

class Object{
	
	public:
		Object(float);
		Object(float,glm::vec3 , glm::vec3); // pos , orientation
		// Struct for _shapes_ (can be multiple in one object)
		struct objShape{
			std::vector<float> Vertices;
			std::vector<float> Normals;
			std::vector<float> TexCoord;
			unsigned int vaoHandle;
			int triangleCount;
			int matId;
		};
		
		// Overarching struct for obj files
		struct objectData{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			
			std::map< std::string,glm::vec3 > minMax;
		};
		
		std::map< std::string,std::vector< objShape > > data;
		objectData objFile;
		objShape emptyObjShape;
		
		std::map<std::string, GLuint> textures;
		std::string base_dir = "";
		
		virtual void updatePos(std::map< std::string , bool > , double);
		
		virtual void printVertices();
		
		float scale;
		void findMinMax();
		virtual void loadFile( std::string input );
		void loadShapes();
		virtual void loadTexture();
		bool FileExists(const std::string& abs_filename);
		void loadTexGPU(std::string); 
		glm::vec3 getPos();
		glm::vec3 getOri();
		
		glm::vec3 pos;
		glm::vec3 rpy;
};

#endif