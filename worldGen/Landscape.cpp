// Jordan Hoskin-Stark
// Replaced by HeightMap
#include "Landscape.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

Landscape::Landscape(float size){
	
	float groundSize = 10.0f;
	generateGround("flat" , groundSize , glm::vec3(-groundSize/2 , 0.0 , groundSize/2) );
	//generateGround("tri" , size , glm::vec3( -0.5 , 0 , 0 ) );
	
}

void Landscape::generateGround(std::string type , float s, glm::vec3 l){
	
	std::vector<float> Vertices;
	
	if (type == "flat"){
	
		// Draw a simple flat surface
		Vertices = {
			0 + l.x, 0 + l.y, 0  + l.z,
			s + l.x, 0 + l.y, 0  + l.z,
			0 + l.x, 0 + l.y, -s  + l.z,
			s + l.x, 0 + l.y, 0  + l.z,
			s + l.x, 0 + l.y, -s  + l.z,
			0 + l.x, 0 + l.y, -s  + l.z
		};
	}
	else if(type == "tri"){
		
		Vertices = {
			0 + l.x, 0 + l.y, 0  + l.z,
			s + l.x, 0 + l.y, 0  + l.z,
			s/2 + l.x, s + l.y, -s/2  + l.z,
			s + l.x, 0 + l.y, 0  + l.z,
			s + l.x, 0 + l.y, -s  + l.z,
			s/2 + l.x, s + l.y, -s/2  + l.z,
			s + l.x, 0 + l.y, -s  + l.z,
			0 + l.x, 0 + l.y, -s  + l.z,
			s/2 + l.x, s + l.y, -s/2  + l.z,
			0 + l.x, 0 + l.y, -s  + l.z,
			0 + l.x, 0 + l.y, 0  + l.z,
			s/2 + l.x, s + l.y, -s/2  + l.z,
		};
		
	}
	
	std::vector<float> Normals = findNormals(Vertices);
	
	std::vector<float> TexCoord = {
		0 , 0 ,
		1 , 0 ,
		0 , 1 ,
		1 , 0 ,
		1 , 1 ,
		0 , 1 , 
		0 , 0 ,
		1 , 0 ,
		0 , 1 ,
		1 , 0 ,
		1 , 1 ,
		0 , 1 ,
		0 , 0 ,
		1 , 0 ,
		0 , 1 ,
		1 , 0 ,
		1 , 1 ,
		0 , 1 ,
		0 , 0 ,
		1 , 0 ,
		0 , 1 ,
		1 , 0 ,
		1 , 1 ,
		0 , 1
	};
	objShape landscape;
	landscape.Vertices = Vertices;
	landscape.Normals = Normals;
	landscape.TexCoord = TexCoord;
	landscape.triangleCount = Vertices.size() / ( 3 * 3 );
	
	data["none"].push_back(landscape);
	
}

std::vector<float> Landscape::findNormals(std::vector<float> vert){
	
	glm::vec3 s1,s2,s3;
	std::vector<float> Normals;
	
	for (int i=0;i<vert.size();i=i+9){
		s1.x = vert.at(i);
		s1.y = vert.at(i+1);
		s1.z = vert.at(i+2);
		
		s2.x = vert.at(i+3);
		s2.y = vert.at(i+4);
		s2.z = vert.at(i+5);
		
		s3.x = vert.at(i+6);
		s3.y = vert.at(i+7);
		s3.z = vert.at(i+8);
		
		
		/* glm::vec3 U = s2-s1;
		glm::vec3 V = s3-s1; */
		
		glm::vec3 N;
		N = glm::normalize(glm::cross(s3 - s1, s2 - s1));
		
		/* N.x = U.y * V.z - U.z * V.y;
		N.y = U.z * V.x - U.x * V.z;
		N.z = U.x * V.y - U.y * V.x; */
		
		Normals.push_back(N.x);
		Normals.push_back(N.y);
		Normals.push_back(N.z);
	}
	
	return Normals;
}
