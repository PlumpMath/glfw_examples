#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "Collision.h"
#include <iostream>

class GLObject
{
public:
	glm::vec3 center_;
	glm::mat4 model_matrix_ = glm::mat4(1.0f);

	//std::vector<float> vertices;
	std::vector<glm::vec3> vertices;

	GLuint vertexbuffer;

	~GLObject()
	{
		glDeleteBuffers(1, &vertexbuffer);
	}

	void genVertexBuffer()
	{
		if (vertices.size() > 0)
		{
			glGenBuffers(1, &vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*vertices.size(), vertices.data(), GL_STATIC_DRAW);
		}
	}

	void drawLineLoop(const GLint& MatrixID, const glm::mat4 vp)
	{
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(vp*model_matrix_)[0][0]);
		
		glEnableVertexAttribArray(0); //TODO: not quite sure if this need to be called before all object drawing

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glVertexAttribPointer
		(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size (x, y, z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
	}

	void drawLines(const GLint& MatrixID, const glm::mat4 vp)
	{
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &(vp*model_matrix_)[0][0]);

		glEnableVertexAttribArray(0); //TODO: not quite sure if this need to be called before all object drawing

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		glVertexAttribPointer
		(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size (x, y, z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_LINES, 0, vertices.size() / 3);
	}

	void rotateCenteredZAxis(const float& angle_degree)
	{
		// TODo; define center
		//model_matrix_ = glm::translate(model_matrix_ , -center_);
		//model_matrix_ = glm::rotate(model_matrix_, glm::radians(angle_degree), glm::vec3(0, 0, 1));
		//model_matrix_ = glm::translate(model_matrix_ , +center_);

		model_matrix_ = glm::translate(center_) * glm::rotate(glm::mat4(), glm::radians(angle_degree), glm::vec3(0, 0, 1)) * glm::translate(-center_) * model_matrix_;
	}

	void checkCollisionLoop(const glm::vec3& ray_start, const glm::vec3& ray_end, int& flag, float& t, glm::vec3& col_pt) const
	{
		Collision col;
		flag = 0;

		//TODO: check t and find closest collision
		float min_t = 1e8;

		for (int i = 0; i < vertices.size(); i++)
		{
			//todo: debug

			glm::vec3 *col_pt_ptr = col.intersection(ray_start, ray_end, vertices[i%vertices.size()], vertices[(i + 1)%vertices.size()]);

			//std::cout << "End pt "<< vertices[i].x<<" "<<vertices[i].y << " ! " << vertices[(i + 1) % vertices.size()].x << " "<< vertices[(i + 1) % vertices.size()].y << std::endl;

			if (col_pt_ptr != nullptr)
			{
				// check if closest collision point
				const float col_t = glm::distance(*col_pt_ptr, ray_start);

				//std::cout << "Col distance "<< col_t << std::endl;

				if (col_t < min_t)
				{
					flag = 1;
					col_pt = *col_pt_ptr;		
					min_t = col_t;
				}		
			}
		}

		if (flag > 0) t = min_t;
	}
};