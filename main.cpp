#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "libraries/openglH.h"
#include "constants.h"

#include "Camera.hpp"
#include "worldGen/Object.hpp"
#include "worldGen/Skybox.hpp"
#include "worldGen/Plane.hpp"
#include "worldGen/Landscape.hpp"
#include "worldGen/HeightMap.hpp"

Camera *camera = new Camera();


// Load the objects in as global variables
// Object(float scale , glm::vec3 pos , glm::vec3 rpy)
// rpy = roll, pitch, yaw in radians from initial orientation

Skybox *skybox = new Skybox(1.0);
HeightMap *ground = new HeightMap(1.0 , "models/heightmap/Heightmap.png", 10.0f);

Plane *plane = new Plane(0.05);
Object *rock = new Object( 0.05 , glm::vec3(1.0 , 0.0 , 0.0) , glm::vec3(0.0) );
Object *cottage = new Object( 0.05 , glm::vec3(0.0 , 0.0 , 0.0) , glm::vec3(0.0 , 0.0 , DEG2RAD(-120)) );
Object *lampPost = new Object( 0.01 , glm::vec3(-0.5 , 0.0 , 0.0) , glm::vec3(0.0) );
Object *tree = new Object( 0.1 , glm::vec3(-0.5 , 0.0 , -1.0) , glm::vec3(0.0 , DEG2RAD(-90) , 0.0) );


std::map< std::string , bool > keyPress;
/*     ** ** ** ** ** **

 ----  Loading functions  ----

       ** ** ** ** ** **      */
	   
void loadShaders(){
	// Load shader and vertex data
	
	programIdMap["debug"] = LoadShaders("shaders/debug_inspect.vert", "shaders/debug_inspect.frag");
	programIdMap["main"] = LoadShaders("shaders/main.vert", "shaders/main.frag");
	programIdMap["skybox"] = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
	
	// Check for errors across the shaders
	for (std::map<std::string,int>::iterator item=programIdMap.begin(); item!=programIdMap.end(); ++item){
		
		if (item->second == 0) {
			std::cerr << "Shader program "<< item->first << " could not be loaded" << std::endl;
			exit(1);
		}
		
	}
}

int loadVao(Object * object){
	
	// Store each shape on the VAO buffer
	for (std::map<std::string,std::vector< Object::objShape > >::iterator item=object->data.begin(); item!=object->data.end(); ++item){
		
		std::vector< Object::objShape > Shapes = item->second;
		
		for (int j=0;j<Shapes.size(); j++){
			Object::objShape shape = Shapes.at(j);
			unsigned int vH;
			
			glGenVertexArrays(1, &vH);
			glBindVertexArray(vH);

			unsigned int buffer[3];
			glGenBuffers(3, buffer);
			
			// Set vertex attributes
			glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
			glBufferData(GL_ARRAY_BUFFER, shape.Vertices.size() * sizeof(float), &shape.Vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);  
			
			if (shape.Normals.size() > 0){
				// Set normal attributes
				glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
				glBufferData(GL_ARRAY_BUFFER, shape.Normals.size() * sizeof(float), &shape.Normals[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);  
			}
			
			if (shape.TexCoord.size() > 0){
				// Texture attributes
				glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
				glBufferData(GL_ARRAY_BUFFER, shape.TexCoord.size() * sizeof(float), &shape.TexCoord[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);
			}	 
			// Un-bind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			// Update the shape with the vao handle
			object->data[item->first].at(j).vaoHandle = vH;
		}
	}
	return 1;
	
}

/*     ** ** ** ** ** **

 ----  Rendering functions  ----

       ** ** ** ** ** **      */

void setProjection(){
	
    glm::mat4 projection;
	
    // glm::perspective(fovy, aspect, near, far)
    projection = glm::perspective(M_PI/3.0, double(winX) / double(winY), 0.2, 100.0); 
	
	for (std::map<std::string,int>::iterator item=programIdMap.begin(); item!=programIdMap.end(); ++item){
	//for (int i=0;i<programIdArr.size();i++){
	
		// Load it to the given shader program
		glUseProgram(item->second);
		
		int projHandle = glGetUniformLocation(item->second, "projection");
		if (projHandle == -1) {
			std::cerr << "Uniform: projection_matrix is not an active uniform label for shader: " << item->first << std::endl;
		}
		
		glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
	}
	
}
	   
void renderGround(){
	
	int programId = programIdMap["debug"];
	
	for (std::map<std::string,std::vector< Object::objShape > >::iterator item=ground->data.begin(); item!=ground->data.end(); ++item){
		
		std::vector< Object::objShape > Shapes = item->second;
		
		for (int i=0;i<Shapes.size();i++){
			
			unsigned int vaoHandle = Shapes.at(i).vaoHandle;
			glUseProgram( programId );
			glBindVertexArray(vaoHandle);
			
			// Assign the view matrix
			int viewHandle = glGetUniformLocation(programId, "viewMatrix");
			glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(camera->getView()) );
			
			// Assign the model view matrix
			int mvHandle = glGetUniformLocation(programId, "modelviewMatrix");
			int normHandle = glGetUniformLocation(programId, "normalMatrix");
			
			glm::mat4 mvMatrix;
			glm::mat3 normMatrix;
			
			//scale and translate
			// Set pos using ground->pos = glm::vec3( x , y , z)
			glm::vec3 pos = ground->getPos();
			mvMatrix = glm::scale(mvMatrix, glm::vec3(ground->scale)); 
			mvMatrix = glm::translate(mvMatrix, glm::vec3(pos.x , pos.y, pos.z));

			glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );
			
			// Calculate the normal transformation based on the current view and the model view
			normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix * camera->getView())));
			glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
				
			int vertexCount = 3 * ( Shapes.at(i).triangleCount );
			glDrawArrays(GL_TRIANGLES,0,vertexCount);
			
		}
	}
	glBindVertexArray(0);
	
}

void renderOverheadLight(tinyobj::material_t* material , int programId){
	
	int ambientMtlHandle = glGetUniformLocation(programId, "mtl_ambient");
	int diffuseMtlHandle = glGetUniformLocation(programId, "mtl_diffuse");
	int specularMtlHandle = glGetUniformLocation(programId, "mtl_specular");
	int shininessHandle = glGetUniformLocation(programId, "shininess");
	
/* 	if (ambientMtlHandle == -1 || diffuseMtlHandle == -1 || specularMtlHandle == -1 || shininessHandle == -1){
		std::cout << "Can't find uniforms for material properties" << std::endl;
		exit(1);
	}
	 */
	// Overhead light source
	int overheadHandle = glGetUniformLocation(programId, "overheadlight_dir");
	int overheadAmbHandle = glGetUniformLocation(programId, "overheadlight_ambient");
	int overheadDiffHandle = glGetUniformLocation(programId, "overheadlight_diffuse");
	int overheadSpecHandle = glGetUniformLocation(programId, "overheadlight_specular");
	
/* 	if (overheadHandle == -1 || overheadAmbHandle == -1 || overheadDiffHandle == -1 || overheadSpecHandle == -1){
		std::cout << "Can't find uniforms for overhead light" << std::endl;
		exit(1);
	} */
	
	
	// Set the material properties based on the given shape info
	glUniform1f(shininessHandle,material->shininess);
	
	glUniform4f(ambientMtlHandle,
			material->ambient[0],
			material->ambient[1],
			material->ambient[2],
			1.0);
	glUniform4f(diffuseMtlHandle,
			material->diffuse[0],
			material->diffuse[1],
			material->diffuse[2],
			1.0);
	glUniform4f(specularMtlHandle,
			material->specular[0],
			material->specular[1],
			material->specular[2],
			1.0);
			
	glm::vec3 ambient = glm::vec3(0.2); // Sets the ambient light value for all lighting modes
	
	// Send the overhead light properties
	float overheadLightDir[4] = { 0.0, -1.0, 0.0, 1.0f };
	glUniform4fv(overheadHandle, 1, overheadLightDir); 
	
	
	glm::vec3 sunRGB = glm::vec3(1.0 , 1.0 , 0.98);
	
	glUniform4f(overheadAmbHandle,ambient.x,ambient.y,ambient.z,1.0);
	glUniform4f(overheadDiffHandle,sunRGB.x,sunRGB.y,sunRGB.z,1.0);
	glUniform4f(overheadSpecHandle,sunRGB.x,sunRGB.y,sunRGB.z,1.0);
	
	
}

void renderAircraftLight(){
	glm::vec3 direction = glm::vec3( 0.0, -1.0, 0.0 ); //Straight down until there is a vector that represents the aircraft orientation
	
}

void activateTextures(int programId , tinyobj::material_t* mat, std::map<std::string, GLuint> textures){
	
	std::string diffTex = mat->diffuse_texname;
	std::string bumpTex = mat->bump_texname;
	std::string specTex = mat->specular_texname;
	
	int enableTexMapHand = glGetUniformLocation(programId, "enableTexMap");
	int enableTexMapHandNorm = glGetUniformLocation(programId, "enableTexMapNorm");
	int enableTexMapHandSpec = glGetUniformLocation(programId, "enableTexMapSpec");
	glUniform1i(enableTexMapHand,0);
	glUniform1i(enableTexMapHandNorm,0);
	glUniform1i(enableTexMapHandSpec,0);
	
	//GLint v;
	
	if (textures.find(diffTex) != textures.end()) {
		glUniform1i(enableTexMapHand,1);
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture(GL_TEXTURE_2D, textures[diffTex]);
		
		//glGetIntegerv(GL_ACTIVE_TEXTURE, &v);
		
		//std::cout << "Bound diff texture: " << diffTex << ",texID = " << textures[diffTex] << ", buffer = " << v <<  std::endl;
	}
			
	if (textures.find(bumpTex) != textures.end()) {
		glUniform1i(enableTexMapHandNorm,1);
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture(GL_TEXTURE_2D, textures[bumpTex]);
		
		//glGetIntegerv(GL_ACTIVE_TEXTURE, &v);
		
		//std::cout << "Bound bump texture: " << bumpTex << ",texID = " << textures[bumpTex] << ", buffer = " << v <<  std::endl;
	}
			
	if (textures.find(specTex) != textures.end()) {
		glUniform1i(enableTexMapHandSpec,1);
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture(GL_TEXTURE_2D, textures[specTex]);
		
		//glGetIntegerv(GL_ACTIVE_TEXTURE, &v);
		
		//std::cout << "Bound spec texture: " << specTex << ",texID = " << textures[specTex] << ", buffer = " << v <<  std::endl;
	}
	glActiveTexture( GL_TEXTURE0 );
}

int setupRender(Object * obj , int programId,std::vector< Object::objShape > Shapes , double dt){
	int vertexCount = 0;
	for (int i=0;i<Shapes.size();i++){
		
		
		int matId = Shapes.at(i).matId;
		tinyobj::material_t* material = &obj->objFile.materials[matId];
		activateTextures(programId, material , obj->textures );
		
		unsigned int vaoHandle = Shapes.at(i).vaoHandle;
		glBindVertexArray(vaoHandle);
		
		// Assign the view matrix
		int viewHandle = glGetUniformLocation(programId, "viewMatrix");
		int mvHandle = glGetUniformLocation(programId, "modelviewMatrix");
		int normHandle = glGetUniformLocation(programId, "normalMatrix");
		
		/* if (viewHandle == -1 || mvHandle == -1 ){
			std::cout << "Can't find uniforms view model" << std::endl;
			std::cout << viewHandle << " , " << mvHandle << " , " << std::endl;
			exit(1);
		}
		 */
		if (dt > -1){
			obj->updatePos(keyPress,dt);
		}
		
		glm::vec3 pos = obj->getPos();
		
		renderOverheadLight(material,programId);
		
		
		glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(camera->getView()) );
		
		// Assign the model view matrix
		
		glm::mat4 mvMatrix;
		glm::mat3 normMatrix;
				
		glm::vec3 rpy = obj->getOri();
		
		mvMatrix = glm::translate(mvMatrix, glm::vec3(pos.x , pos.y, pos.z));
		mvMatrix = glm::scale(mvMatrix, glm::vec3(obj->scale));
		 
		mvMatrix = glm::rotate(mvMatrix, rpy.z , glm::vec3(0.0 , 1.0 , 0.0)); // yaw
		mvMatrix = glm::rotate(mvMatrix, rpy.x , glm::vec3(0.0 , 0.0 , 1.0)); // roll
		mvMatrix = glm::rotate(mvMatrix, rpy.y , glm::vec3(1.0 , 0.0 , 0.0)); // pitch
		
		
		glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );
		
		// Calculate the normal transformation based on the current view and the model view
		normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix * camera->getView())));
		glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
					
		vertexCount = vertexCount + 3 * ( Shapes.at(i).triangleCount );
		
	}
	return vertexCount;
}

void drawObject(Object * obj , double dt){
	
	int programId = programIdMap["main"];
	glUseProgram( programId );
	int vertexCount;
	
	int texHandle = glGetUniformLocation(programId, "texMap");
	int texHandleNorm = glGetUniformLocation(programId, "texMapNormal");
	int texHandleSpec = glGetUniformLocation(programId, "texMapSpec");
	glUniform1i(texHandle,0);
	glUniform1i(texHandleNorm,1);
	glUniform1i(texHandleSpec,2);
	
	for (std::map<std::string,std::vector< Object::objShape > >::iterator item=obj->data.begin(); item!=obj->data.end(); ++item){
		
		vertexCount = 0;
		
		std::vector< Object::objShape > Shapes = item->second;
		
		vertexCount += setupRender(obj , programId , Shapes , dt);
		
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
		
	}
		

	glBindVertexArray(0);
}

void renderSkyBox(){
	
	glDepthMask(GL_FALSE);
	
	int programId = programIdMap["skybox"];
	
	glUseProgram( programId );
	glBindVertexArray(skybox->data["none"][0].vaoHandle);
	
	int viewHandle = glGetUniformLocation(programId, "viewMatrix");
	
	GLint cubeHandle = glGetUniformLocation(programId, "skybox");
	glUniform1i(cubeHandle,0);
			
	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(camera->getView()));  
	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewNoTranslation) );
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->textureID);
	
	int vertexCount = 3 * ( skybox->data["none"][0].triangleCount );
	
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
	
	glDepthMask(GL_TRUE);
	
}

void render( double dt ){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjection();
	
	camera->update(keyPress);
	
	renderSkyBox();
	renderGround();
	
	
	drawObject(cottage,-1);
	drawObject(rock,-1);
	drawObject(lampPost,-1);
	
	tree->pos.x = -0.5;
	tree->pos.z = -1.0;
	
	drawObject(tree,-1);
	
	tree->pos.x = -1.0;
	tree->pos.z = -1.5;
	
	drawObject(tree,-1);
	drawObject(plane,dt);
	
	
	glFlush();
	
}
	   
/*     ** ** ** ** ** **

 ----  Callback functions  ----

       ** ** ** ** ** **      */

void error_callback(int error, const char* description){
    std::cerr << description;
}

void reshape_callback(GLFWwindow *window, int x, int y){
	
    winX = x;
    winY = y;
	//avoid 0 vals
	if(winX == 0){
		winX = 1;
	}
	if(winY == 0){
		winY = 1;
	}

    glViewport(0, 0, x, y); // Set the viewport to the size of the window
	setProjection();
	
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	
	GLint polygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
	
    // Close the application by pressing Esc
    if (action == GLFW_PRESS) {
        switch (key) {
			case GLFW_KEY_ESCAPE:
				// Close the window on escape
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_UP:
				keyPress["up"] = true;
				break;
			case GLFW_KEY_LEFT:
				keyPress["left"] = true;
				break;
			case GLFW_KEY_DOWN:
				keyPress["down"] = true;
				break;
			case GLFW_KEY_RIGHT:
				keyPress["right"] = true;
				break;
			case GLFW_KEY_W:
				keyPress["w"] = true;
				break;
			case GLFW_KEY_A:
				keyPress["a"] = true;
				break;
			case GLFW_KEY_S:
				keyPress["s"] = true;
				break;
			case GLFW_KEY_D:
				keyPress["d"] = true;
				break;
			case GLFW_KEY_0:
				plane->updateVelocity(0.0);
				break;
			case GLFW_KEY_1:
				plane->updateVelocity(1.0);
				break;
			case GLFW_KEY_2:
				plane->updateVelocity(2.0);
				break;
			case GLFW_KEY_3:
				plane->updateVelocity(3.0);
				break;
			case GLFW_KEY_4:
				plane->updateVelocity(4.0);
				break;
			case GLFW_KEY_5:
				plane->updateVelocity(5.0);
				break;
			case GLFW_KEY_6:
				plane->updateVelocity(6.0);
				break;
			case GLFW_KEY_7:
				plane->updateVelocity(7.0);
				break;
			case GLFW_KEY_8:
				plane->updateVelocity(8.0);
				break;
			case GLFW_KEY_9:
				plane->updateVelocity(9.0);
				break;

			case GLFW_KEY_B:
				if ( polygonMode == GL_LINE ) {
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
				}
				else {
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				}
				break;
        }
    }
	if (action == GLFW_RELEASE) {
        switch (key) {
			case GLFW_KEY_UP:
				keyPress["up"] = false;
				break;
			case GLFW_KEY_LEFT:
				keyPress["left"] = false;
				break;
			case GLFW_KEY_DOWN:
				keyPress["down"] = false;
				break;
			case GLFW_KEY_RIGHT:
				keyPress["right"] = false;
				break;
			case GLFW_KEY_W:
				keyPress["w"] = false;
				break;
			case GLFW_KEY_A:
				keyPress["a"] = false;
				break;
			case GLFW_KEY_S:
				keyPress["s"] = false;
				break;
			case GLFW_KEY_D:
				keyPress["d"] = false;
				break;
		}
	}
	
}

int main(int argc, char** argv){

// Get OpenGL running

    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    // Create the window and OpenGL context
	std::string title = "The title of the window";
    GLFWwindow* window = glfwCreateWindow(winX, winY, &title[0], NULL, NULL);
	
    if (!window){
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
		exit(1);
	}
    
	// Initialise OpenGL state
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);

// Setup rendering
	
	loadShaders();

// Load in objects
	loadVao(skybox);
	skybox->loadTexture();
	loadVao(ground);
	// ground->printVertices();
	
	 plane->loadFile("models/A6M_ZERO/A6M_ZERO.obj");
	//plane->loadFile("models/btest/Barrel02.obj");
	loadVao(plane);
	
	rock->loadFile("models/rock/rock_v2.obj");
	loadVao(rock);
	
	cottage->loadFile("models/cottage/cottage.obj");
	loadVao(cottage);
	
	lampPost->loadFile("models/lamp_post/rv_lamp_post_4.obj");
	loadVao(lampPost);
	
	tree->loadFile("models/Treeobj/Tree.obj");
	loadVao(tree);
	
	
// Initialise callbacks
	glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
// Main rendering loop

    double start = glfwGetTime();
    double now;
	
	while (!glfwWindowShouldClose(window)){
		
        now = glfwGetTime();
        render( now - start );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	
// Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
	
    exit(0);
	
	return 0;
}
