#include <iostream>
#include <vector>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLSLProgram.h"
#include "GLTools.h"
#include <cmath>
#include <math.h>
#include <time.h>
#include <ctime>
#include "ParseFile.h"
#define M_PI acos(-1.0)

// Standard window width
const int WINDOW_WIDTH  = 640;
// Standard window height
const int WINDOW_HEIGHT = 480;
// GLUT window id/handle
int glutID = 0;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;
std::clock_t start;

float zNear = 0.1f;
float zFar  = 100.0f;
float p1axis = 2.f*(float)M_PI / 8.f;
float height;
float moonheight;
float speed= 0.1f;
glm::vec3 color2 = glm::vec3(0.0, 1.0, 0.0);
ParseFile sunp;
BOOLEAN renderBoundy;
BOOLEAN rendernormalface;
BOOLEAN rendernormalvertex;


/*
Struct to hold data for object rendering.
*/
struct Object
{
	/* IDs for several buffers. */
	GLuint vao;

	GLuint positionBuffer;
	GLuint colorBuffer;

	GLuint indexBuffer;

	/* Model matrix */
	glm::mat4x4 model;
};

//variable camera-position
struct viewpoint
{
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 up;
};
glm::mat4x4 xmat;
glm::mat4x4 ymat;
glm::mat4x4 zmat;


Object boundy;
Object sun;
Object normalface;
Object normalvertex;
Object planet;
Object planet2;
Object moon1p1;
Object moon2p1;
Object moon3p1;
Object moon4p1;
Object moon5p1;
Object moon6p1;
Object moon7p1;
Object moon8p1;
Object moon9p1;
Object moon10p1;
Object moon1p2;
Object moon2p2;
Object moon3p2;
float rad;

Object lines;
viewpoint viewp;

void refreshMatrix(float radiant)
{
	xmat = {
		1,0,0,0,
		0,cos(radiant),-sin(radiant),0,
		0,sin(radiant),cos(radiant),0,
		0,0,0,1 };
	ymat = { cos(radiant),0,sin(radiant),0,
		0,1,0,0,
		-sin(radiant),0,cos(radiant),0,
		0,0,0,1 };
	zmat = {
		cos(radiant),-sin(radiant),0,0,
		sin(radiant),cos(radiant),0,0,
		0,0,1,0,
		0,0,0,1
	};
	rad = radiant;
}

void renderSolid(Object model)
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * model.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);
	// GLUT: bind vertex-array-object
	// this vertex-array-object must be bound before the glutWireSphere call
	glBindVertexArray(model.vao);
	glLineWidth(1.0f);
	glutSolidSphere(1.0, 10, 10);
	// GLUT: unbind vertex-array-object
	glBindVertexArray(0);
}

void renderLines()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * lines.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(lines.vao);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
void renderSun()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * sun.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 1 triangle.
	glBindVertexArray(sun.vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES,sunp.indices.size()*3, GL_UNSIGNED_SHORT, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}
void renderboundysun()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * boundy.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(boundy.vao);
	glDrawElements(GL_LINES, sunp.boundingbox.indices.size(), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
void renderNormalsFace()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * normalface.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(normalface.vao);
	glDrawElements(GL_LINES, sunp.vertexlist.size()*2, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
void renderNormalsVertex()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * normalvertex.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(normalvertex.vao);
	glDrawElements(GL_LINES, sunp.vertexlist.size()*2 , GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}
void initLines()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	//const std::vector<glm::vec3> vertices = { planet.model * glm::vec4(0,-10.f,0,1), planet.model * glm::vec4(0,10.f,0,1),glm::vec3(0.0f, -1000.0f, 0.0f), glm::vec3(0.0f, 1000.0f, 0.0f), planet2.model *glm::vec4(0.0f, -10.0f, 0,1), planet2.model* glm::vec4(0.0f, 10.0f, 0.f,1) };
	const std::vector<glm::vec3> vertices = {glm::vec3(0.0f, -1000.0f, 0.0f), glm::vec3(0.0f, 1000.0f, 0.0f) };
	const std::vector<glm::vec3> colors = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
	const std::vector<GLushort> indices = { 0, 1,2,3,4,5};
	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &lines.vao);
	glBindVertexArray(lines.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &lines.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lines.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &lines.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lines.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &lines.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lines.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);
}
void initBoundybox()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < sunp.boundingbox.vertices.size(); i++)
	{
		vertices.push_back(glm::vec4(sunp.boundingbox.vertices[i][0], sunp.boundingbox.vertices[i][1], sunp.boundingbox.vertices[i][2], 1));
	}
	std::vector<glm::vec3> colors;
	for (int i = 0; i < sunp.boundingbox.vertices.size(); i++)
	{
		colors.push_back(glm::vec3(0.f,1.f,0.f));
	}
	std::vector<GLushort> indices;
	for (int i = 0; i < sunp.boundingbox.indices.size(); i++)
	{
		indices.push_back(sunp.boundingbox.indices[i]);
	}
	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &boundy.vao);
	glBindVertexArray(boundy.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &boundy.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boundy.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &boundy.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boundy.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &boundy.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundy.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);
}
void initNormalsFace()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < sunp.vertexlist.size(); i++)
	{
		vertices.push_back(glm::vec4(sunp.vertexlist[i]->x, sunp.vertexlist[i]->y, sunp.vertexlist[i]->z, 1));
		vertices.push_back(glm::vec4(sunp.vertexlist[i]->x + sunp.vertexlist[i]->edge->face->normal.x, sunp.vertexlist[i]->y + sunp.vertexlist[i]->edge->face->normal.y, sunp.vertexlist[i]->z + sunp.vertexlist[i]->edge->face->normal.z, 1));
	}
	std::vector<glm::vec3> colors;
	for (int i = 0; i < sunp.vertexlist.size(); i++)
	{
		colors.push_back(glm::vec3(0.f, 1.f, 1.f));
		colors.push_back(glm::vec3(0.f, 1.f, 1.f));
	}
	std::vector<GLushort> indices;
	for (int i = 0; i < sunp.vertexlist.size()*2; i++)
	{
		indices.push_back((GLushort)i);
	}
	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &normalface.vao);
	glBindVertexArray(normalface.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &normalface.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalface.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &normalface.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalface.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &normalface.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalface.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);
}
void initNormalsVertex()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < sunp.vertexlist.size(); i++)
	{
		vertices.push_back(glm::vec4(sunp.vertexlist[i]->x, sunp.vertexlist[i]->y, sunp.vertexlist[i]->z, 1));
		vertices.push_back(glm::vec4(sunp.vertexlist[i]->x + sunp.vertexlist[i]->normal.x, sunp.vertexlist[i]->y + sunp.vertexlist[i]->normal.y, sunp.vertexlist[i]->z + sunp.vertexlist[i]->normal.z, 1));
	}
	std::vector<glm::vec3> colors;
	for (int i = 0; i < sunp.vertexlist.size(); i++)
	{
		colors.push_back(glm::vec3(1.f, 0.6f, 0.f));
		colors.push_back(glm::vec3(1.f, 0.6f, 0.f));
	}
	std::vector<GLushort> indices;
	for (int i = 0; i < sunp.vertexlist.size() * 2; i++)
	{
		indices.push_back((GLushort)i);
	}
	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &normalvertex.vao);
	glBindVertexArray(normalvertex.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &normalvertex.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalvertex.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &normalvertex.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalvertex.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &normalvertex.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalvertex.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);
}
void initSolid(Object &model)
{

	// set attribute locations (of shader) for GLUT
	GLuint programId = program.getHandle();
	// position attribute to variable "position"
	glutSetVertexAttribCoord3(glGetAttribLocation(programId, "position"));
	// normal attribute to variable "color"
	// this creates a colorful sphere :-)
	//glutSetVertexAttribNormal(glGetAttribLocation(programId, "color"));
	// create a vertex-array-object for GLUT geometry
	glGenVertexArrays(1, &model.vao);


	// Modify model matrix.
	model.model = glm::mat4(1.0f);
}

void initSun()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	std::vector<glm::vec3> tvertices;
	for (int i = 0; i < sunp.vertices.size(); i++)
	{
		tvertices.push_back(glm::vec3(sunp.vertices[i][0],sunp.vertices[i][1],sunp.vertices[i][2]));
	}
	const std::vector<glm::vec3> vertices = tvertices;
	std::vector<glm::vec3> tcolors;
	for (int i = 0; i < sunp.vertices.size(); i++)
	{
		tcolors.push_back(glm::vec3((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX));
	}
	const std::vector<glm::vec3> colors = tcolors;
	std::vector<GLushort> tindices;
	for (int i = 0; i < sunp.indices.size(); i++)
	{
		tindices.push_back((GLushort)sunp.indices[i][0]-1);
		tindices.push_back((GLushort)sunp.indices[i][1]-1);
		tindices.push_back((GLushort)sunp.indices[i][2]-1);
	}
	const std::vector<GLushort> indices = tindices;
	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &sun.vao);
	glBindVertexArray(sun.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &sun.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sun.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &sun.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sun.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &sun.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sun.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	sun.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void initall()
{

	initSun();
	initBoundybox();
	initNormalsFace();
	initNormalsVertex();
	initSolid(planet);
	planet.model = glm::translate(planet.model, glm::vec3(20, 0, 0));
	refreshMatrix(2 * M_PI / 8);
	planet.model = planet.model * zmat;
	initSolid(planet2);
	planet2.model = glm::translate(planet2.model, glm::vec3(-10, 0, 0));
	initSolid(moon1p1);
	moon1p1.model = planet.model;
	moon1p1.model = glm::scale(moon1p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon1p1.model = glm::translate(moon1p1.model, glm::vec3(10,0,0));
	initSolid(moon2p1);
	moon2p1.model = planet.model;
	moon2p1.model = glm::scale(moon2p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon2p1.model = glm::translate(moon2p1.model, glm::vec3(-10, 0, 0));
	initSolid(moon3p1);
	moon3p1.model = planet.model;
	moon3p1.model = glm::scale(moon3p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon3p1.model = glm::translate(moon3p1.model, glm::vec3(-10, 5, 0));
	initSolid(moon4p1);
	moon4p1.model = planet.model;
	moon4p1.model = glm::scale(moon4p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon4p1.model = glm::translate(moon4p1.model, glm::vec3(10, 5, 0));
	initSolid(moon5p1);
	moon5p1.model = planet.model;
	moon5p1.model = glm::scale(moon5p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon5p1.model = glm::translate(moon5p1.model, glm::vec3(0, 5, -10));
	initSolid(moon6p1);
	moon6p1.model = planet.model;
	moon6p1.model = glm::scale(moon6p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon6p1.model = glm::translate(moon6p1.model, glm::vec3(0, 5, 10));
	initSolid(moon7p1);
	moon7p1.model = planet.model;
	moon7p1.model = glm::scale(moon7p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon7p1.model = glm::translate(moon7p1.model, glm::vec3(-10, -5, 0));
	initSolid(moon8p1);
	moon8p1.model = planet.model;
	moon8p1.model = glm::scale(moon8p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon8p1.model = glm::translate(moon8p1.model, glm::vec3(10, -5, 0));
	initSolid(moon9p1);
	moon9p1.model = planet.model;
	moon9p1.model = glm::scale(moon9p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon9p1.model = glm::translate(moon9p1.model, glm::vec3(0, -5, -10));
	initSolid(moon10p1);
	moon10p1.model = planet.model;
	moon10p1.model = glm::scale(moon10p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon10p1.model = glm::translate(moon10p1.model, glm::vec3(0, -5, 10));
	initSolid(moon1p2);
	moon1p2.model = planet2.model;
	moon1p2.model = glm::scale(moon1p2.model, glm::vec3(0.5, 0.5, 0.5));
	moon1p2.model = glm::translate(moon1p2.model, glm::vec3(cos(2*M_PI/3)*10, 0, sin(2*M_PI/3)*10));
	initSolid(moon2p2);
	moon2p2.model = planet2.model;
	moon2p2.model = glm::scale(moon2p2.model, glm::vec3(0.5, 0.5, 0.5));
	moon2p2.model = glm::translate(moon2p2.model, glm::vec3(cos(2 * M_PI / 3*2) * 10, 0, sin(2 * M_PI / 3*2) * 10));
	initSolid(moon3p2);
	moon3p2.model = planet2.model;
	moon3p2.model = glm::scale(moon3p2.model, glm::vec3(0.5, 0.5, 0.5));
	moon3p2.model = glm::translate(moon3p2.model, glm::vec3(cos(2 * M_PI) * 10, 0, sin(2 * M_PI) * 10));
}

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL: Set "background" color and enable depth testing.
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	viewp.eye = { 0.0f,15.0f,40.0f };
	viewp.center = { 0.0f,0.0f,0.f };
	viewp.up = { 0.0f,1.0f,0.0f };
	view = glm::lookAt(viewp.eye, viewp.center, viewp.up);
	refreshMatrix(0.1);
	// Create a shader program and set light direction.
	if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX))
	{
		std::cerr << program.log();
		return false;
	}

	if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT))
	{
		std::cerr << program.log();
		return false;
	}
	
	if (!program.link())
	{
		std::cerr << program.log();
		return false;
	}

	// Create objects.
	initall();
	initLines();
	return true;
}

void doSomething()
{
	static float p1rad = { rad };
	if (p1rad >= 2 * M_PI) { p1rad = p1rad - 2 * M_PI; }
	//Sun
	sun.model[3][1] = height;
	//Planet1
	glm::mat4x4 backup = planet.model;
	planet.model = planet.model * glm::inverse(planet.model);
	planet.model = glm::rotate(planet.model, p1axis, glm::vec3(0, 0, 1));
	//x-coordinate
	planet.model[3][0] = cos(p1rad)*20;
	//z-coordinate
	planet.model[3][2] = sin(p1rad)*20;
	planet.model = glm::rotate(planet.model, p1rad, glm::vec3(0, 1, 0));
	//y-coordinate
	planet.model[3][1] = height;
	
	//Moon1
	moon1p1.model = planet.model;
	moon1p1.model = glm::scale(moon1p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon1p1.model = glm::rotate(moon1p1.model,p1rad,glm::vec3(0,1,0));
	moon1p1.model = glm::translate(moon1p1.model, glm::vec3(10, 0, 0));

	//Moon2 
	moon2p1.model = planet.model;
	moon2p1.model = glm::scale(moon2p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon2p1.model = glm::rotate(moon2p1.model, p1rad, glm::vec3(0, 1, 0));
	moon2p1.model = glm::translate(moon2p1.model, glm::vec3(-10, 0, 0));
	//Moon3 
	moon3p1.model = planet.model;
	moon3p1.model = glm::scale(moon3p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon3p1.model = glm::rotate(moon3p1.model, p1rad, glm::vec3(0, 1, 0));
	moon3p1.model = glm::translate(moon3p1.model, glm::vec3(-10, 5, 0));
	//Moon4
	moon4p1.model = planet.model;
	moon4p1.model = glm::scale(moon4p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon4p1.model = glm::rotate(moon4p1.model, p1rad, glm::vec3(0, 1, 0));
	moon4p1.model = glm::translate(moon4p1.model, glm::vec3(10, 5, 0));
	//Moon5
	moon5p1.model = planet.model;
	moon5p1.model = glm::scale(moon5p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon5p1.model = glm::rotate(moon5p1.model, p1rad, glm::vec3(0, 1, 0));
	moon5p1.model = glm::translate(moon5p1.model, glm::vec3(0,5,-10));
	//Moon6 
	moon6p1.model = planet.model;
	moon6p1.model = glm::scale(moon6p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon6p1.model = glm::rotate(moon6p1.model, p1rad, glm::vec3(0, 1, 0));
	moon6p1.model = glm::translate(moon6p1.model, glm::vec3(0, 5, 10));
	//Moon7 
	moon7p1.model = planet.model;
	moon7p1.model = glm::scale(moon7p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon7p1.model = glm::rotate(moon7p1.model, p1rad, glm::vec3(0, 1, 0));
	moon7p1.model = glm::translate(moon7p1.model, glm::vec3(-10, -5, 0));
	//Moon8
	moon8p1.model = planet.model;
	moon8p1.model = glm::scale(moon8p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon8p1.model = glm::rotate(moon8p1.model, p1rad, glm::vec3(0, 1, 0));
	moon8p1.model = glm::translate(moon8p1.model, glm::vec3(10, -5, 0));
	//Moon9
	moon9p1.model = planet.model;
	moon9p1.model = glm::scale(moon9p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon9p1.model = glm::rotate(moon9p1.model, p1rad, glm::vec3(0, 1, 0));
	moon9p1.model = glm::translate(moon9p1.model, glm::vec3(0, -5, -10));
	//Moon10 
	moon10p1.model = planet.model;
	moon10p1.model = glm::scale(moon10p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon10p1.model = glm::rotate(moon10p1.model, p1rad, glm::vec3(0, 1, 0));
	moon10p1.model = glm::translate(moon10p1.model, glm::vec3(0, -5, 10));
	//Planet2
	planet2.model = planet2.model * glm::inverse(planet2.model) * ymat * planet2.model;
	planet2.model = planet2.model * ymat;
	planet2.model[3][1] = height+moonheight;
	//Moon1
	moon1p2.model = glm::translate(glm::translate(moon1p2.model, glm::vec3(-cos(2 * M_PI / 3) * 10, 0, -sin(2 * M_PI / 3) * 10))*glm::inverse(ymat)*glm::inverse(ymat), glm::vec3(cos(2 * M_PI / 3) * 10, 0, sin(2 * M_PI / 3) * 10));
	moon1p2.model = moon1p2.model * glm::inverse(moon1p2.model) * ymat * moon1p2.model;
	moon1p2.model[3][1] = height+moonheight;
	//Moon2
	moon2p2.model = glm::translate(glm::translate(moon2p2.model, glm::vec3(-cos(2 * M_PI / 3*2) * 10, 0, -sin(2 * M_PI / 3*2) * 10))*glm::inverse(ymat)*glm::inverse(ymat), glm::vec3(cos(2 * M_PI / 3*2) * 10, 0, sin(2 * M_PI / 3*2) * 10));
	moon2p2.model = moon2p2.model * glm::inverse(moon2p2.model) * ymat * moon2p2.model;
	moon2p2.model[3][1] = height+moonheight;
	//Moon3
	moon3p2.model = glm::translate(glm::translate(moon3p2.model, glm::vec3(-cos(2 * M_PI) * 10, 0, -sin(2 * M_PI) * 10))*glm::inverse(ymat)*glm::inverse(ymat), glm::vec3(cos(2 * M_PI) * 10, 0, sin(2 * M_PI) * 10));
	moon3p2.model = moon3p2.model * glm::inverse(moon3p2.model) * ymat * moon3p2.model;
	moon3p2.model[3][1] = height+moonheight;
	boundy.model = sun.model;
	normalface.model = sun.model;
	normalvertex.model = sun.model;
	p1rad += rad;
}


/*
 Release object resources.
*/
void releaseObject(Object& obj)
{
	glDeleteVertexArrays(1, &obj.vao);
	glDeleteBuffers(1, &obj.indexBuffer);
	glDeleteBuffers(1, &obj.colorBuffer);
	glDeleteBuffers(1, &obj.positionBuffer);
}

/*
 Release resources on termination.
 */
void release()
{
	releaseObject(boundy);
	releaseObject(sun);
	releaseObject(normalface);
	releaseObject(normalvertex);
	releaseObject(planet);
	releaseObject(planet2);
	releaseObject(moon1p1);
	releaseObject(moon2p1);
	releaseObject(moon3p1);
	releaseObject(moon4p1);
	releaseObject(moon5p1);
	releaseObject(moon6p1);
	releaseObject(moon7p1);
	releaseObject(moon8p1);
	releaseObject(moon9p1);
	releaseObject(moon10p1);
	releaseObject(moon1p2);
	releaseObject(moon2p2);
	releaseObject(moon3p2);
}


void renderall()
{
	renderSun();
	if (renderBoundy) { renderboundysun(); }
	if (rendernormalface) { renderNormalsFace(); }
	if (rendernormalvertex) { renderNormalsVertex(); }
	renderSolid(planet);
	renderSolid(planet2);
	renderSolid(moon1p1);
	renderSolid(moon2p1);
	renderSolid(moon3p1);
	renderSolid(moon4p1);
	renderSolid(moon5p1);
	renderSolid(moon6p1);
	renderSolid(moon7p1);
	renderSolid(moon8p1);
	renderSolid(moon9p1);
	renderSolid(moon10p1);
	renderSolid(moon1p2);
	renderSolid(moon2p2);
	renderSolid(moon3p2);

}
/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	renderall();
	renderLines();
}
void refresh()
{
	//std::cout << view[3][2];
	doSomething();
	renderall();
}

void glutDisplay ()
{
   GLCODE(render());
   glutSwapBuffers();
   refreshMatrix(speed*(std::clock() - start)/50);
   start = std::clock();
   refresh();
}


/*
 Resize callback.
 */
void glutResize (int width, int height)
{
	// Division by zero is bad...
	height = height < 1 ? 1 : height;
	glViewport(0, 0, width, height);

	// Construct projection matrix.
	projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
	const float radiant = 0.1;
	//z-rotation-matrix
	glm::mat4x4 zmat = {
		cos(radiant),-sin(radiant),0,0,
		sin(radiant),cos(radiant),0,0,
		0,0,1,0,
		0,0,0,1
	};
	//x-rotation-matrix
	glm::mat4x4 xmat = {
		1,0,0,0,
		0,cos(radiant),-sin(radiant),0,
		0,sin(radiant),cos(radiant),0,
		0,0,0,1
	};
	//y-rotation-matrix
	glm::mat4x4 ymat = {
		cos(radiant),0,sin(radiant),0,
		0,1,0,0,
		-sin(radiant),0,cos(radiant),0,
		0,0,0,1
	};
	glm::mat4x4 backupx;
	glm::mat4x4 backupy;
	glm::mat4x4 backupz;
	switch (keycode)
	{
	case 27: // ESC
	  glutDestroyWindow ( glutID );
	  return;
	case 'p':
		p1axis += 0.1;
		if (p1axis >= 2 * M_PI) { p1axis = p1axis - 2 * M_PI; }
		break;
	case 'P':
		p1axis -= 0.1;
		if (p1axis <= 0) { p1axis = 2*M_PI - p1axis; }
		break;
	case 'i':
		if (renderBoundy == true)
		{
			renderBoundy = false;
		}
		else
		{
			renderBoundy = true;
		}
		break;
	case 'n':
		if (rendernormalface == true)
		{
			rendernormalface = false;
		}
		else
		{
			rendernormalface = true;
		}
		break;
	case 'h':
		if (rendernormalvertex == true)
		{
			rendernormalvertex = false;
		}
		else
		{
			rendernormalvertex = true;
		}
		break;
	case 't':
		height += 0.1f;
		break;
	case 'T':
		height -= 0.1f;
		break;
	case 'l':
		moonheight += 0.1f;
		break;
	case 'L':
		moonheight -= 0.1f;
		break;
	case 'w':
		speed += 0.01f;
		break;
	case 'W':
		speed -= 0.01f;
		break;
	case 'a':
		viewp.eye.z += 0.1;
		view = glm::lookAt(viewp.eye, viewp.center, viewp.up);
		break;
	case 's':
		viewp.eye.z -= 0.1;
		view = glm::lookAt(viewp.eye, viewp.center, viewp.up);
		break;
	case 'x':
		
		backupx = sun.model;
		sun.model = glm::inverse(sun.model)*sun.model;
		sun.model = sun.model * xmat;
		sun.model = sun.model * backupx;
		break;
	case 'y':
		
		backupy = sun.model;
		sun.model = glm::inverse(sun.model)*sun.model;
		sun.model = sun.model * ymat;
		sun.model = sun.model * backupy;
		break;
	case 'z':
		
		backupz = sun.model;
		sun.model = glm::inverse(sun.model)*sun.model;
		sun.model = sun.model * zmat;
		sun.model = sun.model * backupz;
		break;
	}
	
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	sunp.parseFile("Testobjs/cone_maya.obj");
	//sunp.parseFile("Testobjs/Superspaceship.obj");
	//sunp.parseFile("Testobjs/cone_vtvn.obj");
	//sunp.parseFile("Testobjs/cone.obj");
	//sunp.parseFile("Testobjs/A4_testcube2_mitSpitze.obj");
	//sunp.parseFile("Testobjs/dodecahedron.obj");
	//sunp.parseFile("Testobjs/stanford_bunny_closed.obj");
	//sunp.getHalfEdge();
	//sunp.checkConsistence();

	// GLUT: Initialize freeglut library (window toolkit).
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(40,40);
	glutInit(&argc, argv);

	// GLUT: Create a window and opengl context (version 4.3 core profile).
	glutInitContextVersion(4, 3);
	glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutCreateWindow("Aufgabenblatt X");
	glutID = glutGetWindow();
	  
	// GLEW: Load opengl extensions
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();

	if (result != GLEW_OK) {
	   return -1;
	}

	// GLUT: Set callbacks for events.
	glutReshapeFunc(glutResize);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc   (glutDisplay); // redisplay when idle
	glutKeyboardFunc(glutKeyboard);
	start = std::clock();
	// Init VAO.
	{
		GLCODE(bool result = init());
		if (!result) {
			release();
			return -2;
		}
	}

	// GLUT: Loop until the user closes the window
	// rendering & event handling
	glutMainLoop ();
	// Clean up everything on termination.
	release();
	

	return 0;
}
