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

#include "worldGen/Landscape.hpp"
#include "worldGen/Object.hpp"


Landscape *ground = new Landscape(10);

// Quick camera for now

glm::vec3 initEye(0.0f , 0.5f , 1.0f);
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
	unsigned int vaoHandle = ground->data.at(0).vaoHandle;
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
		
	int vertexCount = 3 * ( ground->data.at(0).triangleCount );
	glDrawArrays(GL_TRIANGLES,0,vertexCount);
	
}

void render( double dt ){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjection();
	
	renderGround();
	
	
	glBindVertexArray(0);
	glFlush();
	
}
	   
/*     ** ** ** ** ** **

 ----  Callback functions  ----

       ** ** ** ** ** **      */

void error_callback(int error, const char* description){
    std::cerr << description;
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
	
	ground->printVertices();
	loadVao(ground);
	
// Initialise callbacks


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


/*#include "worldGen/Landscape.hpp"

 int main(){

	Landscape ground = Landscape(10);
	
	ground.printVertices();
	
	return 0;
} */