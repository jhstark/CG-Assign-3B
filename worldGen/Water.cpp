#include "Water.hpp"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

Water::Water(float scale, float size): Object(scale){
    pos = glm::vec3(-0.1, -0.0345, -1.0);
    generate(size);
}

//finds normals
std::vector<float> Water::findNormals(std::vector<float> vert){
	
	glm::vec3 s1,s2,s3;
	std::vector<float> Normals;
    glm::vec3 N;
    std::vector<float> current(3);

    //for every triangle, calculate normal for each vertex
    for(int i=0; i<vert.size()-8; i=i+9){
        s1.x = vert.at(i);
	    s1.y = vert.at(i+1);
	    s1.z = vert.at(i+2);
        s2.x = vert.at(i+3);
        s2.y = vert.at(i+4);
	    s2.z = vert.at(i+5);
        s3.x = vert.at(i+6);
	    s3.y = vert.at(i+7);
	    s3.z = vert.at(i+8);

        N = glm::normalize(glm::cross(s3 - s1, s2 - s1));

        current[0] = s1.x;
        current[1] = s1.y;
        current[2] = s1.z;
        nMap[current].push_back(N);
        current[0] = s2.x;
        current[1] = s2.y;
        current[2] = s2.z;
        nMap[current].push_back(N);
        current[0] = s3.x;
        current[1] = s3.y;
        current[2] = s3.z;
        nMap[current].push_back(N);
    }

    //for every vertex, calculate average normal
    for(int i=0; i<vert.size()-2; i=i+3){
        std::vector<float> current(3);
        std::vector<glm::vec3> cNormal;
        glm::vec3 norm = glm::vec3(0, 0, 0);

        current[0] = vert[i];
        current[1] = vert[i+1];
        current[2] = vert[i+2];

        //get vector of normals for vertex
        cNormal = nMap[current];
        float s = cNormal.size();

        //sum all normals
        for(int j=0; j<cNormal.size(); j++){
            norm += cNormal[j];
        }

        //average
        norm = norm/s;

        //add to normals
        Normals.push_back(norm.x);
        Normals.push_back(norm.y);
        Normals.push_back(norm.z);

    }
	
	return Normals;
}

//generates texCoords from a vector of vertices
std::vector<float> Water::generateTexCoords(std::vector<float> vert, float size){
    std::vector<float> TexCoords;
    glm::vec2 current;

    for (int i=0;i<vert.size();i=i+3){
        current.x = vert.at(i)/size;
        current.y = vert.at(i+2)/size;

        TexCoords.push_back(current.x);
        TexCoords.push_back(current.y);
    }

    return TexCoords;
}

float Water::proceduralGeom(float x, float z){
    float result;

    result = (sin(x)+1)*0.0005 + (cos(z)+1)*0.0025;
    return result;
}

void Water::generate(float size){
    std::vector<float> Vertices;
    float width = 200;
    float height = 200;
    float yVal = 0.0;
    float yVal2 = 0.0;
    float tileSize = size/width;

    //for centering
    glm::vec3 o(-size/2, 0.0, size/2);

    for(int x = 0; x<width-1; x++){

        for(int y = 0; y<height-1; y++){
            //first triangle
            //top left
            Vertices.push_back(o.x + float(x)*tileSize);
            Vertices.push_back(o.y + proceduralGeom(x, y));
            Vertices.push_back(o.z + float(y)*tileSize);
            //bottom left
            Vertices.push_back(o.x + float(x)*tileSize);
            Vertices.push_back(o.y + proceduralGeom(x, y+1));
            Vertices.push_back(o.z + float(y+1)*tileSize);
            //bottom right
            Vertices.push_back(o.x + float(x+1)*tileSize);
            Vertices.push_back(o.y + proceduralGeom(x+1, y+1));
            Vertices.push_back(o.z + float(y+1)*tileSize);

            //second triangle
            //top left
            Vertices.push_back(o.x + float(x)*tileSize);
            Vertices.push_back(o.y + proceduralGeom(x, y));
            Vertices.push_back(o.z + float(y)*tileSize);
            //bottom right
            Vertices.push_back(o.x + float(x+1)*tileSize);
            Vertices.push_back(o.y + proceduralGeom(x+1, y+1));
            Vertices.push_back(o.z + float(y+1)*tileSize);
            //top right
            Vertices.push_back(o.x + float(x+1)*tileSize);
            Vertices.push_back(o.y + proceduralGeom(x+1, y));
            Vertices.push_back(o.z + float(y)*tileSize);
        }
    }

    //normals
    std::vector<float> Normals = findNormals(Vertices);

    //tex coords
    std::vector<float> TexCoord = generateTexCoords(Vertices, size);

    objShape water;
	water.Vertices = Vertices;
	water.Normals = Normals;
	water.TexCoord = TexCoord;
	water.triangleCount = Vertices.size() / 9;
	
	data["none"].push_back(water);



}