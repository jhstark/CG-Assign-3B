#include "HeightMap.hpp"

#include "../libraries/stb_image.h"

#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

HeightMap::HeightMap(float scale , std::string image, float size): Object(scale){
    
    loadHeightMap(image);
    generate(size);
	
	rpy.z =  -1.57; // -90degrees
	pos.x = -1.0;
    if(width < 2){
        std::cerr << "Image should have width > 1." << std::endl;
        exit(1);
    }
}

//loads image data from specified file, convert to w x h vector
void HeightMap::loadHeightMap(std::string image){
    int w, h;
    int comp;
    unsigned char* pixelOffset;
    unsigned char r;

    //check if image specified
    if (image.length() > 0) {

        //if file doesn't exist
        if (!FileExists(image)) {
              std::cerr << "Unable to find file: " << image << std::endl;
              exit(1);
        }

        unsigned char* data = stbi_load(image.c_str(), &w, &h, &comp, 3);
        if (!data) {
            std::cerr << "Unable to load height map: " << image << std::endl;
            exit(1);
        }

        heightMap.resize(w);

        for (int i = 0; i < w; i++) {
            for(int y = 0; y < h; y++){
                pixelOffset = data + (i + h * y) * 3;
                r = pixelOffset[0];
                heightMap[i].push_back((int)r);
            }
        }
        width = w;
        height = h;
    }
    else{
        std::cerr << "No height map specified." << std::endl;
        exit(1);
    }
}

//finds normals
std::vector<float> HeightMap::findNormals(std::vector<float> vert){
	
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

//generates texCoords from a vector of vertices
std::vector<float> HeightMap::generateTexCoords(std::vector<float> vert, float size){
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

//generates a landscape from heightmap using specified size
//center point is 0,0, will have width * height vertices
void HeightMap::generate(float size){
    std::vector<float> Vertices;
    float tileSize = size/width;

    //for centering
    glm::vec3 o(-size/2, 0.0, size/2);

    //generate grid
    for(int x = 0; x<width-1; x++){
        for(int y = 0; y<height-1; y++){
            //first triangle
            //top left
            Vertices.push_back(o.x + float(x)*tileSize);
            Vertices.push_back(o.y + tileSize*float(heightMap[x][y]));
            Vertices.push_back(o.z + float(y)*tileSize);
            //bottom left
            Vertices.push_back(o.x + float(x)*tileSize);
            Vertices.push_back(o.y + tileSize*float(heightMap[x][y+1]));
            Vertices.push_back(o.z + float(y+1)*tileSize);
            //bottom right
            Vertices.push_back(o.x + float(x+1)*tileSize);
            Vertices.push_back(o.y + tileSize*float(heightMap[x+1][y+1]));
            Vertices.push_back(o.z + float(y+1)*tileSize);

            //second triangle
            //top left
            Vertices.push_back(o.x + float(x)*tileSize);
            Vertices.push_back(o.y + tileSize*float(heightMap[x][y]));
            Vertices.push_back(o.z + float(y)*tileSize);
            //bottom right
            Vertices.push_back(o.x + float(x+1)*tileSize);
            Vertices.push_back(o.y + tileSize*float(heightMap[x+1][y+1]));
            Vertices.push_back(o.z + float(y+1)*tileSize);
            //top right
            Vertices.push_back(o.x + float(x+1)*tileSize);
            Vertices.push_back(o.y + tileSize*float(heightMap[x+1][y]));
            Vertices.push_back(o.z + float(y)*tileSize);
        }
    }

    std::vector<float> Normals = findNormals(Vertices);

    //tex coords
    std::vector<float> TexCoord = generateTexCoords(Vertices, size);

    objShape heightmap;
	heightmap.Vertices = Vertices;
	heightmap.Normals = Normals;
	heightmap.TexCoord = TexCoord;
	heightmap.triangleCount = Vertices.size() / 9;
	
	data["none"].push_back(heightmap);
}