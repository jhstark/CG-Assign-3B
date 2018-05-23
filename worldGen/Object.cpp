#include "Object.hpp"
#include <iostream>
#include <vector>

void Object::printVertices(){
	int i;
	for (int j = 0 ; j < data.size() ; j++){
		
		std::cout << "Shape " << j << std::endl;
		objShape shape = data.at(i);
		
		std::cout << " -> Vertices:" << std::endl;
		for (i=0;i<shape.Vertices.size();i=i+3){
			std::cout << "\t(" << shape.Vertices.at(i) << "," << shape.Vertices.at(i+1) << "," << shape.Vertices.at(i+2) << ")" << std::endl;
		}
		
		std::cout << " -> Normals:" << std::endl;
		for (i=0;i<shape.Normals.size();i=i+3){
			std::cout << "\t(" << shape.Normals.at(i) << "," << shape.Normals.at(i+1) << "," << shape.Normals.at(i+2) << ")" << std::endl;
		}
		
		std::cout << " -> TexCoord:" << std::endl;
		for (i=0;i<shape.TexCoord.size();i=i+2){
			std::cout << "\t(" << shape.TexCoord.at(i) << "," << shape.TexCoord.at(i+1) << ")" << std::endl;
		}
		
		std::cout << " -> Triangle Count:" << std::endl;
			std::cout << "\t" << shape.triangleCount << std::endl;
		
	}
}