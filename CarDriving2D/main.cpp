/*

http://www.opengl-tutorial.org/beginners-tutorials/
https://github.com/opengl-tutorials/ogl

#include <stdio.h> error fix : http://stackoverflow.com/questions/38290169/cannot-find-corecrt-h-universalcrt-includepath-is-wrong
Add directories

*/

#include "GLFWExample.h"
#include "GLSquare.h"
#include "GLLineSegments.h"
#include "SelfDrivingCar.h"
#include <memory>

GLFWExample glfw_example;

int main(void)
{
	glfw_example.init();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	SelfDrivingCar my_car;

	// world (-1.0, -0.5) x (2.0, 1.5)
	std::vector<std::unique_ptr<GLObject>> obj_list;
	obj_list.push_back(std::move(std::unique_ptr<GLSquare>(new GLSquare(glm::vec3(0.5f, 0.5f, 0.0f), 1.5f, 1.0f)))); // world
	obj_list.push_back(std::move(std::unique_ptr<GLSquare>(new GLSquare(glm::vec3(1.2f, 0.5f, 0.0f), 0.1f, 0.2f))));
	obj_list.push_back(std::move(std::unique_ptr<GLSquare>(new GLSquare(glm::vec3(-0.3f, 0.5f, 0.0f), 0.1f, 0.2f))));
	obj_list.push_back(std::move(std::unique_ptr<GLSquare>(new GLSquare(glm::vec3(0.3f, 1.1f, 0.0f), 0.3f, 0.05f))));
	obj_list.push_back(std::move(std::unique_ptr<GLSquare>(new GLSquare(glm::vec3(0.6f, -0.25f, 0.0f), 0.3f, 0.25f))));


	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
		//glm::mat4 Projection = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,100.0f); // In world coordinates

		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(0.5, 0.5, 3), // Camera is at (4,3,3), in World Space
			glm::vec3(0.5, 0.5, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// animate update
		//my_car.car_body.rotateCenteredZAxis(1);

		if (glfw_example.getKeyPressed(GLFW_KEY_LEFT) == true) my_car.turnLeft();
		if (glfw_example.getKeyPressed(GLFW_KEY_RIGHT) == true) my_car.turnRight();
		if (glfw_example.getKeyPressed(GLFW_KEY_UP) == true) my_car.accel();
		if (glfw_example.getKeyPressed(GLFW_KEY_DOWN) == true) my_car.decel();

		my_car.update();

		my_car.updateSensor(obj_list);

		// car-object collision
		if (my_car.car_body.checkCollisionLoop(obj_list) == true)
		{
			static int count = 0;

			std::cout << "Collision "<< count ++<< std::endl;
			my_car.init();
			my_car.car_body.model_matrix_ = glm::mat4(1.0f);
		}

		// car-world escape check
		//for (auto itr : my_car.car_body.vertices)
		//{
		//	const glm::vec4 v4 = my_car.car_body.model_matrix_ * glm::vec4(itr.x, itr.y, itr.z, 1.0f);

		//	if (world_bb.isInside(v4.x, v4.y) == false) std::cout << "World outside " << v4.x << " " << v4.y << std::endl;
		//}

		// draw
		my_car.car_body.drawLineLoop(MatrixID, Projection * View);
		my_car.sensing_lines.drawLineLoop(MatrixID, Projection * View);		

		//for (auto itr : obj_list) // this doesn't work with unique ptr
		for(int i = 0; i < obj_list.size(); i ++)
		{
			obj_list[i]->drawLineLoop(MatrixID, Projection * View);
		}

		glDisableVertexAttribArray(0);

		glfw_example.swapBuffers();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfw_example.getKeyPressed(GLFW_KEY_ESCAPE) &&	glfw_example.getWindowShouldClose());

	// Cleanup VBO
	//glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
