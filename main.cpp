#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "openglH.h"
#include "constants.h"

#include "worldGen/Object.hpp"
#include "worldGen/Plane.hpp"
#include "worldGen/Landscape.hpp"

// Load the objects in as global variables
// NOTE: if you can think of a better way to do this for the render please do it!

Landscape *ground = new Landscape( 0.5 );
Plane *plane = new Plane();

// Quick camera for now

glm::vec3 initEye(0.0f , 0.5f , 2.0f);
glm::vec3 at(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

glm::mat4 viewMtx = glm::lookAt(initEye, at, up);

/*     ** ** ** ** ** **

 ----  Loading functions  ----

       ** ** ** ** ** **      */
	   
void loadShaders(){
	// Load shader and vertex data
	
	programIdMap["debug"] = LoadShaders("shaders/debug_inspect.vert", "shaders/debug_inspect.frag");
	
	// Check for errors across the shaders
	for (std::map<std::string,int>::iterator item=programIdMap.begin(); item!=programIdMap.end(); ++item){
		
		if (item->second == 0) {
			std::cerr << "Shader program "<< item->first << " could not be loaded" << std::endl;
			exit(1);
		}
		
	}
}

int loadVao(Object * object){
	//
	//std::vector<unsigned int> vaoVect;
	std::vector< Object::objShape > Shapes = object->data;
	// Store each shape on the VAO buffer
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
		
		// Set normal attributes
		glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, shape.Normals.size() * sizeof(float), &shape.Normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);  

		// Texture attributes
		glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
		glBufferData(GL_ARRAY_BUFFER, shape.TexCoord.size() * sizeof(float), &shape.TexCoord[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);
					 
		// Un-bind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		// Update the shape with the vao handle
		object->data.at(j).vaoHandle = vH;
    }
	
	return 1;
	
}

/*     ** ** ** ** ** **

 ----  Rendering functions  ----

       ** ** ** ** ** **      */

void setProjection(){
	
    glm::mat4 projection;
	
    // glm::perspective(fovy, aspect, near, far)
    projection = glm::perspective(M_PI/3.0, double(winX) / double(winY), 0.2, 30.0); 
	
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
	
	for (int i=0;i<ground->data.size();i++){
		
		unsigned int vaoHandle = ground->data.at(i).vaoHandle;
		glUseProgram( programId );
		glBindVertexArray(vaoHandle);
		
		// Assign the view matrix
		int viewHandle = glGetUniformLocation(programId, "view");
		glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMtx) );
		
		// Assign the model view matrix
		int mvHandle = glGetUniformLocation(programId, "modelviewMatrix");
		int normHandle = glGetUniformLocation(programId, "normalMatrix");
		
		glm::mat4 mvMatrix;
		glm::mat3 normMatrix;
		
		mvMatrix = glm::scale(mvMatrix, glm::vec3(1.0)); 
		glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );
		
		// Calculate the normal transformation based on the current view and the model view
		normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix * viewMtx)));
		glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
		
		// Assign the colour value
		int diffuseHandle = glGetUniformLocation(programId, "Kd");
		
		if ( diffuseHandle == -1 ){
			std::cout << "Can't find uniforms colour diffuse" << std::endl;
			exit(1);
		}
		
		glUniform3f(diffuseHandle, 0.5 , 1.0 , 0.5);
			
		int vertexCount = 3 * ( ground->data.at(i).triangleCount );
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
		
	}
	
	glBindVertexArray(0);
	
}
void renderPlane(double dt){
	
	int programId = programIdMap["debug"];
	
	for (int i=0;i<plane->data.size();i++){
		
		unsigned int vaoHandle = plane->data.at(i).vaoHandle;
		glUseProgram( programId );
		glBindVertexArray(vaoHandle);
		
		// Assign the view matrix
		int viewHandle = glGetUniformLocation(programId, "view");
		glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMtx) );
		
		// Assign the model view matrix
		int mvHandle = glGetUniformLocation(programId, "modelviewMatrix");
		int normHandle = glGetUniformLocation(programId, "normalMatrix");
		
		glm::mat4 mvMatrix;
		glm::mat3 normMatrix;
		
		// Calculate the model scale and normal transformation matrices
		float screenScale = 0.8; // percentage of window object should take up
		float scaleMultiplier = screenScale / ( plane->scale() );
		
		
		plane->updatePos(dt);
		
		glm::vec3 pos = plane->getPos();
		
		float ori = plane->getOrientation();
		
		mvMatrix = glm::scale(mvMatrix, glm::vec3(scaleMultiplier));
		mvMatrix = glm::rotate(mvMatrix, ori , glm::vec3(0.0 , 1.0 , 0.0)); 
		mvMatrix = glm::rotate(mvMatrix, ori , glm::vec3(0.0 , 0.0 , -0.5)); 
		
		
		mvMatrix = glm::translate(mvMatrix, glm::vec3(pos.x , pos.y, pos.z)); 
		
		glUniformMatrix4fv(mvHandle, 1, false, glm::value_ptr(mvMatrix) );
		
		// Calculate the normal transformation based on the current view and the model view
		normMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix * viewMtx)));
		glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
		
		// Assign the colour value
		int diffuseHandle = glGetUniformLocation(programId, "Kd");
		
		if ( diffuseHandle == -1 ){
			std::cout << "Can't find uniforms colour diffuse" << std::endl;
			exit(1);
		}
		
		glUniform3f(diffuseHandle, 0.5 , 1.0 , 0.5);
			
		int vertexCount = 3 * ( plane->data.at(i).triangleCount );
		glDrawArrays(GL_TRIANGLES,0,vertexCount);
		
	}

	glBindVertexArray(0);

}

void render( double dt ){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjection();
	
	// renderGround();
	renderPlane(dt);
	
	
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
	loadVao(ground);
	plane->loadFile("models/A6M_ZERO/A6M_ZERO.obj");
	//plane->loadFile("models/testbarrel/Barrel02.obj");
	
	//plane->printVertices();
	
	loadVao(plane);
	
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
