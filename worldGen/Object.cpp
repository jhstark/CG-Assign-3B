#include "Object.hpp"
#include <iostream>
#include <vector>
#include <map>
#include "../libraries/openglH.h"

void Object::printVertices(){
	int i;
	
	for (std::map<std::string,std::vector< Object::objShape > >::iterator item=data.begin(); item!=data.end(); ++item){
		std::vector< Object::objShape > Shapes = item->second;	
		for (int j = 0 ; j < Shapes.size() ; j++){
			
			std::cout << "Shape " << j << std::endl;
			objShape shape = Shapes.at(j);
			
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
}

// Returns the highest axis distance to scale by
float Object::scale(){

	float dist;
	
	float minX = objFile.minMax["min"].x;
	float minY = objFile.minMax["min"].y;
	float minZ = objFile.minMax["min"].z;
	
	float maxX = objFile.minMax["max"].x;
	float maxY = objFile.minMax["max"].y;
	float maxZ = objFile.minMax["max"].z;
	
	float distX = std::abs(maxX - minX);
	float distY = std::abs(maxY - minY);
	float distZ = std::abs(maxZ - minZ);
	

	// Return the largest distance
	dist = distX;
	
	if (distY > dist){
		dist = distY;
	}	
	
	if (distZ > dist){
		dist = distZ;
	}	
	
	return dist;
	
}

void Object::findMinMax(){

	std::map< std::string,glm::vec3 > minMax;
	
	float x,y,z;
	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(0.0f);
	int first = true;
	for (std::map<std::string,std::vector< Object::objShape > >::iterator item=data.begin(); item!=data.end(); ++item){
		
		std::vector< Object::objShape > Shapes = item->second;	
		for (int j=0;j<Shapes.size(); j++){

			std::vector<float> Vertices = Shapes.at(j).Vertices;

			for (int i=0;i<Vertices.size();i = i + 3){	

				x = Vertices.at(i);
				y = Vertices.at(i + 1);
				z = Vertices.at(i + 2);
				
				if ( first ){
					min.x = x;
					min.y = y;
					min.z = z;
					
					max.x = x;
					max.y = y;
					max.z = z;
					first = false;
				}
				// Assign the min and max values	
				if (x < min.x){
					min.x = x;
				}
				if (x > max.x){
					max.x = x;
				}
				
				if (y < min.y){
					min.y = y;
				}
				if (y > max.y){
					max.y = y;
				}
				
				if (z < min.z){
					min.z = z;
				}
				if (z > max.z){
					max.z = z;
				}
				
			}
		}
	}
	
	minMax["min"] = min;
	minMax["max"] = max;
	
	objFile.minMax = minMax;
	
}

void Object::loadFile( std::string input ){
	std::cout << input << std::endl;
	std::string err;
	
	// Thanks to https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
	if (input.find_last_of("/\\") != std::string::npos){
		base_dir = input.substr(0, input.find_last_of("/\\"));
	}
	
	if (base_dir.empty()) {
		base_dir = ".";
	}
	#ifdef _WIN32
		base_dir += "\\";
	#else
		base_dir += "/";
	#endif
	
	bool ret = tinyobj::LoadObj(&objFile.attrib, &objFile.shapes, &objFile.materials, &err, input.c_str(), base_dir.c_str());
	
	if (!err.empty()){
		std::cerr << err << std::endl;
	}
	
	if (!ret){
		exit(1);
	}
	
	loadShapes();
	// Find and store the minimum and maximum vertices for normalisation
	findMinMax();
	loadTexture();
}

bool Object::FileExists(const std::string& abs_filename) {
  bool ret;
  FILE* fp = fopen(abs_filename.c_str(), "rb");
  if (fp) {
    ret = true;
    fclose(fp);
  } else {
    ret = false;
  }

  return ret;
}

void Object::loadTexture(){
	
	
	for (size_t m = 0; m < objFile.materials.size(); m++) {
      tinyobj::material_t* mp = &objFile.materials[m];

      if (mp->diffuse_texname.length() > 0) {
        // Only load the texture if it is not already loaded
        if (textures.find(mp->diffuse_texname) == textures.end()) {
          GLuint texture_id;
          int w, h;
          int comp;

          std::string texture_filename = mp->diffuse_texname;
          if (!FileExists(texture_filename)) {
            // Append base dir.
            texture_filename = base_dir + mp->diffuse_texname;
            if (!FileExists(texture_filename)) {
              std::cerr << "Unable to find file: " << mp->diffuse_texname
                        << std::endl;
              exit(1);
            }
          }

          unsigned char* image =
              stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
          if (!image) {
            std::cerr << "Unable to load texture: " << texture_filename
                      << std::endl;
            exit(1);
          }
          std::cout << "Loaded texture: " << texture_filename << ", w = " << w
                    << ", h = " << h << ", comp = " << comp << std::endl;

          glGenTextures(1, &texture_id);
          glBindTexture(GL_TEXTURE_2D, texture_id);
		  
		std::cout << "Bound "<< texture_filename <<" to " << texture_id << std::endl;
		
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          if (comp == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, image);
          } else if (comp == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, image);
          } else {
            assert(0);  // TODO
          }
          glBindTexture(GL_TEXTURE_2D, 0);
          stbi_image_free(image);
          textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
        }
      }
	}
}

void Object::loadShapes(){
	
	for (size_t s = 0; s < objFile.shapes.size(); s++) {
		
		objShape shape;
		int triangleCount = 0;
		
		// Loop over faces(polygon)
		size_t index_offset = 0;
		std::string lastTexName;
		std::string texName;
		int materialId = -1;
		for (size_t f = 0; f < objFile.shapes[s].mesh.num_face_vertices.size(); f++) {
			
			int fv = objFile.shapes[s].mesh.num_face_vertices[f];
			
			// per-face material
			materialId = objFile.shapes[s].mesh.material_ids[f];
			
			texName = objFile.materials[materialId].diffuse_texname;
			
			// If the material changes this will treat it as a new shape
			if (f>0 && lastTexName != texName){
		
				shape.triangleCount = triangleCount;
				triangleCount = 0;
				// Send the shape to the global var
				shape.matId = materialId;
				data[lastTexName].push_back(shape);
				
				// Reset shape
				shape = emptyObjShape;
				
				
			}
			
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {

				tinyobj::index_t idx = objFile.shapes[s].mesh.indices[index_offset + v];
				
				tinyobj::real_t vx = objFile.attrib.vertices[3*idx.vertex_index+0];
				tinyobj::real_t vy = objFile.attrib.vertices[3*idx.vertex_index+1];
				tinyobj::real_t vz = objFile.attrib.vertices[3*idx.vertex_index+2];
				
				shape.Vertices.push_back(vx);
				shape.Vertices.push_back(vy);
				shape.Vertices.push_back(vz);
				
				tinyobj::real_t nx , ny , nz = 0;
				
				if (objFile.attrib.normals.size() > 0){
					nx = objFile.attrib.normals[3*idx.normal_index+0];
					ny = objFile.attrib.normals[3*idx.normal_index+1];
					nz = objFile.attrib.normals[3*idx.normal_index+2];
				}
				
				shape.Normals.push_back(nx);
				shape.Normals.push_back(ny);
				shape.Normals.push_back(nz);

				tinyobj::real_t tx,ty = 0;

				if (objFile.attrib.texcoords.size() > 0){

					tx = objFile.attrib.texcoords[2*idx.texcoord_index+0];
					ty = objFile.attrib.texcoords[2*idx.texcoord_index+1];
				
				}

				shape.TexCoord.push_back(tx);
				shape.TexCoord.push_back(ty);
			
				// Keep track of the number of primitives per shape for drawing
				triangleCount++;
			}
			
			
			index_offset += fv;
			lastTexName = texName;
		}
		
		shape.triangleCount = triangleCount;

		// Send the shape to the global var
		shape.matId = materialId;
		data[texName].push_back(shape);
		
	}
}