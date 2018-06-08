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

// Load the objects in as global variables
// NOTE: if you can think of a better way to do this for the render please do it!

Camera *camera = new Camera();

Skybox *skybox = new Skybox();
Landscape *ground = new Landscape( 0.5 );
Plane *plane = new Plane();


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
			int viewHandle = glGetUniformLocation(programId, "view");
			glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(camera->getView()) );
			
			// Assign the model view matrix
			int mvHandle = glGetUniformLocation(programId, "modelviewMatrix");
			int normHandle = glGetUniformLocation(programId, "normalMatrix");
			
			glm::mat4 mvMatrix;
			glm::mat3 normMatrix;
			
			mvMatrix = glm::scale(mvMatrix, glm::vec3(1.0)); 
			glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );
			
			// Calculate the normal transformation based on the current view and the model view
			normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix * camera->getView())));
			glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
			
			// Assign the colour value
			int diffuseHandle = glGetUniformLocation(programId, "Kd");
			
			if ( diffuseHandle == -1 ){
				std::cout << "Can't find uniforms colour diffuse" << std::endl;
				exit(1);
			}
			
			glUniform3f(diffuseHandle, 0.5 , 1.0 , 0.5);
				
			int vertexCount = 3 * ( Shapes.at(i).triangleCount );
			glDrawArrays(GL_TRIANGLES,0,vertexCount);
			
		}
	}
	glBindVertexArray(0);
	
}
void renderPlane(double dt){
	
	int programId = programIdMap["main"];
	int vertexCount = 0;
	for (std::map<std::string,std::vector< Object::objShape > >::iterator item=plane->data.begin(); item!=plane->data.end(); ++item){
		
		std::vector< Object::objShape > Shapes = item->second;
		
			
		std::string diffuse_texname = item->first;
		if (plane->textures.find(diffuse_texname) != plane->textures.end()) {
			glBindTexture(GL_TEXTURE_2D, plane->textures[diffuse_texname]);
		}
		
		for (int i=0;i<Shapes.size();i++){
			
			unsigned int vaoHandle = Shapes.at(i).vaoHandle;
			glUseProgram( programId );
			glBindVertexArray(vaoHandle);
			
			int texHandle = glGetUniformLocation(programId, "texMap");
			
			// Assign the view matrix
			int viewHandle = glGetUniformLocation(programId, "viewMatrix");
			int mvHandle = glGetUniformLocation(programId, "modelviewMatrix");
			int normHandle = glGetUniformLocation(programId, "normalMatrix");
			
			if (viewHandle == -1 || mvHandle == -1 ){
				std::cout << "Can't find uniforms view model" << std::endl;
				std::cout << viewHandle << " , " << mvHandle << " , " << std::endl;
				exit(1);
			}
			
			glUniform1i(texHandle,0);
			
			plane->updatePos(keyPress,dt);
			
			glm::vec3 pos = plane->getPos();
			camera->lookAt(pos);
			
			glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(camera->getView()) );
			
			// Assign the model view matrix
			
			glm::mat4 mvMatrix;
			glm::mat3 normMatrix;
			
			// Calculate the model scale and normal transformation matrices
			float screenScale = 0.8; // percentage of window object should take up
			float scaleMultiplier = screenScale / ( plane->scale() );
			
			glm::vec3 rpy = plane->getOrientation();
			mvMatrix = glm::scale(mvMatrix, glm::vec3(scaleMultiplier));
			mvMatrix = plane->getModelMat();
			/* mvMatrix = glm::translate(mvMatrix, glm::vec3(pos.x , pos.y, pos.z)); 
			mvMatrix = glm::rotate(mvMatrix, rpy.z , glm::vec3(1.0 , 0.0 , 0.0)); 
			mvMatrix = glm::rotate(mvMatrix, rpy.x , glm::vec3(0.0 , 0.0 , 1.0)); 
			mvMatrix = glm::rotate(mvMatrix, rpy.y , glm::vec3(0.0 , 1.0 , 0.0)); */
			
			
			glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );
			
			// Calculate the normal transformation based on the current view and the model view
			normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix * camera->getView())));
			glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
						
			vertexCount = vertexCount + 3 * ( Shapes.at(i).triangleCount );
			
		}
		
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
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	int vertexCount = 3 * ( skybox->data["none"][0].triangleCount );
	
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
	
	glDepthMask(GL_TRUE);
	
}

void render( double dt ){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjection();
	
	renderSkyBox();
	renderGround();
	//renderPlane(dt);
	
	
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
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

// Setup rendering
	
	loadShaders();

// Load in objects
	loadVao(skybox);
	loadVao(ground);
	plane->loadFile("models/A6M_ZERO/A6M_ZERO.obj");
	//plane->loadFile("models/btest/Barrel02.obj");
	loadVao(plane);
	
	//plane->printVertices();
	
	
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
