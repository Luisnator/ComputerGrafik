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
#include "Object.h"
#include <cmath>
#include <math.h>
#include <time.h>
#include <ctime>
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


/*
Struct to hold data for object rendering.
*/
struct Objectt
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

Object *test = new Object();
Objectt quad;
Objectt sun;
Objectt planet;
Objectt planet2;
Objectt moon1p1;
Objectt moon2p1;
Objectt moon3p1;
Objectt moon4p1;
Objectt moon5p1;
Objectt moon6p1;
Objectt moon7p1;
Objectt moon8p1;
Objectt moon9p1;
Objectt moon10p1;
Objectt moon1p2;
Objectt moon2p2;
Objectt moon3p2;
float rad;

Objectt lines;
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

void renderWire(Object model)
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * *model.getModel();

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// GLUT: bind vertex-array-object
	// this vertex-array-object must be bound before the glutWireSphere call
	glBindVertexArray(*model.getVao());

	//glLineWidth(1.0f);
	glutWireSphere(1.0, 10, 10);

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


void initLines()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	const std::vector<glm::vec3> vertices = { planet.model * glm::vec4(0,-10.f,0,1), planet.model * glm::vec4(0,10.f,0,1),glm::vec3(0.0f, -1000.0f, 0.0f), glm::vec3(0.0f, 1000.0f, 0.0f), planet2.model *glm::vec4(0.0f, -10.0f, 0,1), planet2.model* glm::vec4(0.0f, 10.0f, 0.f,1) };
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

void initWire(Object &model)
{

	// set attribute locations (of shader) for GLUT
	GLuint programId = program.getHandle();
	// position attribute to variable "position"
	glutSetVertexAttribCoord3(glGetAttribLocation(programId, "position"));
	// normal attribute to variable "color"
	// this creates a colorful sphere :-)
	glutSetVertexAttribNormal(glGetAttribLocation(programId, "color"));
	// create a vertex-array-object for GLUT geometry
	glGenVertexArrays(1, model.getVao());

	// Modify model matrix.
	*model.getModel() = glm::mat4(1.0f);
}

void initall()
{
	initWire(*test);
	
}

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL: Set "background" color and enable depth testing.
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
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
	
}


/*
 Release object resources.
*/
void releaseObject(Objectt& obj)
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
	// Shader program will be released upon program termination.
	releaseObject(quad);
}


void renderall()
{
	renderWire(*test);
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
	//old transformation
	glm::mat4x4 backup = quad.model;
	//inverse transform-matrix
	glm::mat4x4 inverse = glm::inverse(quad.model);

	switch (keycode)
	{
	case 27: // ESC
	  glutDestroyWindow ( glutID );
	  return;
	}
	
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	// GLUT: Initialize freeglut library (window toolkit).
        glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
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
	//glutIdleFunc   (glutDisplay); // redisplay when idle
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
