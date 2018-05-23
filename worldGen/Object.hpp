#ifndef _OBJECT_HPP
#define _OBJECT_HPP

#include <iostream>
#include <vector>

class Object{
	
	public:		
		typedef struct{
			std::vector<float> Vertices;
			std::vector<float> Normals;
			std::vector<float> TexCoord;
			unsigned int vaoHandle;
			int triangleCount;
			int matId;
		} objShape;
		
		std::vector< objShape > data;

		virtual void printVertices();
		
};

#endif