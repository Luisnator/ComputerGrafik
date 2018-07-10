#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// OpenGL, GLSL
#include "GLTools.h"
#include "GLSLProgram.h"
#include "TextureObject.h"
#include "ParseFile.h"
#include "Object.h"
#include "AbstractCube.h"

namespace cg
{
	class ThingTextured : public AbstractCube
	{
	public:

		ParseFile p;
		ThingTextured(const std::string& texname) {
		  p.parseFile("Testobjs/Superspaceshipsuper.obj");
			texture.initTexture(texname);			
			initShader();
			initObject();
		}

		void initShader()
		{
			ThingTextured::initShader(programShaded, "shader/textured.vert", "shader/textured.frag");

			programShaded.use();
			programShaded.setUniform("imgTexture", 0);
		}

		void initObject ()
		{
		  // top:    0, 1, 4, 5
		  // bottom: 2, 3, 6, 7

			std::vector<glm::vec3> vertices;
			for (int i = 0; i < p.vertices.size(); i++)
			{
				vertices.push_back(glm::vec3(p.vertices[i][0], p.vertices[i][1], p.vertices[i][2]));
			}
			std::vector<glm::vec2> tvertices;
			for (int i = 0; i < p.texturcoord.size(); i++)
			{
				tvertices.push_back(p.texturcoord[i]);
			}
			
			// Build the cube.
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texcoords;
			std::vector<GLuint> indices;

			
			// 0, 1, 2, 3*
		/*	CubeTextured::addTriangle(positions, normals, texcoords,
					      vertices[0],  vertices[1],  vertices[2],
					      tvertices[0], tvertices[1], tvertices[2]);*/ 
			for (int i = 0; i < p.indices.size(); i++)
			{
				positions.push_back(vertices[p.indices[i][0] - 1]);
				positions.push_back(vertices[p.indices[i][1] - 1]);
				positions.push_back(vertices[p.indices[i][2] - 1]);
				normals.push_back(glm::normalize(vertices[p.indices[i][0] - 1]));
				normals.push_back(glm::normalize(vertices[p.indices[i][1] - 1]));
				normals.push_back(glm::normalize(vertices[p.indices[i][2] - 1]));
			}
			for (int i = 0; i < p.inditex.size(); i++)
			{
				texcoords.push_back(tvertices[p.inditex[i][0] -1]);
				texcoords.push_back(tvertices[p.inditex[i][1] -1]);
				texcoords.push_back(tvertices[p.inditex[i][2] -1]);
			}
			cout << "fertig";
			for (GLushort i = 0; i < positions.size(); i++)
				indices.push_back(i);

			GLuint programId = programShaded.getHandle();
			GLuint pos;

			// Vertex array object.
			glGenVertexArrays(1, &objCube.vao);
			glBindVertexArray(objCube.vao);

			// Position buffer.
			glGenBuffers(1, &objCube.positionBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, objCube.positionBuffer);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

			pos = glGetAttribLocation(programId, "position");
			glEnableVertexAttribArray(pos);
			glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

			// Normal buffer.
			glGenBuffers(1, &objCube.normalBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, objCube.normalBuffer);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

			pos = glGetAttribLocation(programId, "normal");
			glEnableVertexAttribArray(pos);
			glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

			// Color buffer.
			glGenBuffers(1, &objCube.colorBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, objCube.colorBuffer);
			glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), texcoords.data(), GL_STATIC_DRAW);

			pos = glGetAttribLocation(programId, "texCoord");
			glEnableVertexAttribArray(pos);
			glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);


			// Index buffer.
			objCube.indexCount = (GLuint)indices.size();

			glGenBuffers(1, &objCube.indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objCube.indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, objCube.indexCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		}

		~ThingTextured() {
			// GLSL Programs and objects will be automatically released upon object destruction.
		}

		void render(const glm::mat4& view, const glm::mat4& projection)
		{
		        glm::mat4 modelview = view * model;
			// Create mvp.
			glm::mat4 mvp = projection * modelview;
			// Create normal matrix (nm) from model matrix.
			glm::mat3 nm = glm::inverseTranspose(glm::mat3(model));

			// Bind texture object to unit GL_TEXTURE0
			texture.use();
			
			// Bind the shader program and set uniform(s).
			programShaded.use();
			programShaded.setUniform("mvp", mvp);
			programShaded.setUniform("normalMatrix",     nm);
			programShaded.setUniform("modelviewMatrix",  modelview);
			programShaded.setUniform("projectionMatrix", projection);

			// Bind vertex array object so we can render the triangle.
			glBindVertexArray(objCube.vao);
			glDrawElements(GL_TRIANGLES, objCube.indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Unbind texture object to unit GL_TEXTURE0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture  (GL_TEXTURE_2D, 0); 
		}

		TextureObject texture;  
	private:
		static void addQuad(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texcoords,
				    const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d,
				    const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc, const glm::vec2& td)
		{
		  addTriangle(positions, normals, texcoords, a, c, b, ta, tc, tb);
		  addTriangle(positions, normals, texcoords, a, d, c, ta, td, tc);		       
		}

		static void addTriangle(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texcoords,
					const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
					const glm::vec2& ta, const glm::vec2& tb, const glm::vec2& tc)
		{
			// Add vertices, normals and colors to the buffers.
			positions.push_back(a);
			positions.push_back(b);
			positions.push_back(c);

			// This is simple... this time!
			normals.push_back(glm::normalize(a));
			normals.push_back(glm::normalize(b));
			normals.push_back(glm::normalize(c));

			texcoords.push_back(ta);
			texcoords.push_back(tb);
			texcoords.push_back(tc);
		}
		
		static void initShader(GLSLProgram& program, const std::string& vert, const std::string& frag)
		{
			if (!program.compileShaderFromFile(vert.c_str(), cg::GLSLShader::VERTEX))
			{
				throw std::runtime_error("COMPILE VERTEX: " + program.log());
			}

			if (!program.compileShaderFromFile(frag.c_str(), cg::GLSLShader::FRAGMENT))
			{
				throw std::runtime_error("COMPILE FRAGMENT: " + program.log());
			}

			if (!program.link())
			{
				throw std::runtime_error("LINK: " + program.log());
			}
		}

	private:
		GLSLProgram programShaded;
		
		Object        objCube;
	};
}
