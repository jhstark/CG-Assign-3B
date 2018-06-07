#ifndef _OBJECT_HPP
#define _OBJECT_HPP

#include "../libraries/stb_image.h"
#include "../libraries/tiny_obj_loader.h"

#include <iostream>
#include <vector>
#include <map>
#include "../openglH.h"

class Object{
	
	public:
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
		
		std::vector< objShape > data;
		objectData objFile;
		std::map<std::string, GLuint> textures;
		std::string base_dir = "";
		
		virtual void printVertices();
		float scale();
		void findMinMax();
		virtual void loadFile( std::string input );
		void loadShapes();
		void loadTexture();
		bool FileExists(const std::string& abs_filename);
};

#endif