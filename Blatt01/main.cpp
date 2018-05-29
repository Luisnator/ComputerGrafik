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


Object quad;
Object sun;
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
}

void renderQuad(Object model)
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * model.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);
	
	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(model.vao);
	glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_SHORT, 0);
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

void initQuad(Object &model)
{

	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	const std::vector<glm::vec3> vertices = { { -1.0f, 1.0f, -1.0f }, { -1.0, -1.0, -1.0 }, { 1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, -1.0f }/*Front*/,{1.0f, -1.0f,-1.0f},{1.0f,1.0f,-1.0f },{1.0f,-1.0f,1.0f}, {1.0f,1.0f,1.0f}/*RightSide*/,{-1,-1,-1},{1,-1,-1},{1,-1,1},{-1,-1,1}/*Bottom*/,{-1,-1,-1},{-1,-1,1},{-1,1,-1},{-1,1,1}/*LeftSide*/,{-1,-1,1},{1,-1,1},{-1,1,1},{1,1,1}/*Back*/,{-1,1,-1},{1,1,-1},{1,1,1},{-1,1,1}/*Top*/};
	const std::vector<glm::vec3> colors = { { 0.f, 1.f, 0.f }, { 0.f, 1.f,0.f}, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f }/*Front*/,{ 0.f, 1.f, 1.f },{ 0.f, 1.f,1.f },{ 0.f, 1.f, 1.f },{ 0.f, 1.f, 1.f }/*RideSide*/ ,{ 0.5f, 1.f, 0.f },{ 0.5f, 1.f,0.f },{ 0.5f, 1.f, 0.f },{ 0.5f, 1.f, 0.f }/*Bottom*/ ,{ 1.f, 0.5f, 0.f },{ 1.f, 0.5f,0.f },{ 1.f, 0.5f, 0.f },{ 1.f, 0.5f, 0.f }/*LeftSide*/ ,{ 0.f, 1.f, 0.5f },{ 0.f, 1.f,0.5f },{ 0.f, 1.f, 0.5f },{ 0.f, 1.f, 0.5f }/*Back*/,{ 0.7f, 1.f, 0.3f },{ 0.7f, 1.f,0.3f },{ 0.7f, 1.f, 0.3f },{ 0.7f, 1.f, 0.3f }/*Top*/ };
	const std::vector<GLushort> indices = { 0, 1, 2,/**/ 0, 2, 3/*Front*/,4,5,6,/**/6,7,5/*RightSide*/,8,9,10,/**/10,11,8/*Bottom*/,12,13,14,/**/14,15,13/*LeftSide*/,16,17,18/**/,18,19,17/*Back*/,20,21,22/**/,22,23,20/*Top*/ };


	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &model.vao);
	glBindVertexArray(model.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &model.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, model.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &model.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, model.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &model.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	model.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void initall()
{

	initQuad(sun);
	initQuad(planet);
	planet.model = glm::translate(planet.model, glm::vec3(15, 0, 0));
	refreshMatrix(2 * M_PI / 8);
	planet.model = planet.model * zmat;
	initQuad(planet2);
	planet2.model = glm::translate(planet2.model, glm::vec3(-15, 0, 0));
	initQuad(moon1p1);
	moon1p1.model = planet.model;
	moon1p1.model = glm::scale(moon1p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon1p1.model = glm::translate(moon1p1.model, glm::vec3(10,0,0));
	initQuad(moon2p1);
	moon2p1.model = planet.model;
	moon2p1.model = glm::scale(moon2p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon2p1.model = glm::translate(moon2p1.model, glm::vec3(-10, 0, 0));
	initQuad(moon3p1);
	moon3p1.model = planet.model;
	moon3p1.model = glm::scale(moon3p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon3p1.model = glm::translate(moon3p1.model, glm::vec3(-10, 5, 0));
	initQuad(moon4p1);
	moon4p1.model = planet.model;
	moon4p1.model = glm::scale(moon4p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon4p1.model = glm::translate(moon4p1.model, glm::vec3(10, 5, 0));
	initQuad(moon5p1);
	moon5p1.model = planet.model;
	moon5p1.model = glm::scale(moon5p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon5p1.model = glm::translate(moon5p1.model, glm::vec3(0, 5, -10));
	initQuad(moon6p1);
	moon6p1.model = planet.model;
	moon6p1.model = glm::scale(moon6p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon6p1.model = glm::translate(moon6p1.model, glm::vec3(0, 5, 10));
	initQuad(moon7p1);
	moon7p1.model = planet.model;
	moon7p1.model = glm::scale(moon7p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon7p1.model = glm::translate(moon7p1.model, glm::vec3(-10, -5, 0));
	initQuad(moon8p1);
	moon8p1.model = planet.model;
	moon8p1.model = glm::scale(moon8p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon8p1.model = glm::translate(moon8p1.model, glm::vec3(10, -5, 0));
	initQuad(moon9p1);
	moon9p1.model = planet.model;
	moon9p1.model = glm::scale(moon9p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon9p1.model = glm::translate(moon9p1.model, glm::vec3(0, -5, -10));
	initQuad(moon10p1);
	moon10p1.model = planet.model;
	moon10p1.model = glm::scale(moon10p1.model, glm::vec3(0.5, 0.5, 0.5));
	moon10p1.model = glm::translate(moon10p1.model, glm::vec3(0, -5, 10));
	initQuad(moon1p2);
	moon1p2.model = planet2.model;
	moon1p2.model = glm::scale(moon1p2.model, glm::vec3(0.5, 0.5, 0.5));
	moon1p2.model = glm::translate(moon1p2.model, glm::vec3(-10, 0, 10));
	initQuad(moon2p2);
	moon2p2.model = planet2.model;
	moon2p2.model = glm::scale(moon2p2.model, glm::vec3(0.5, 0.5, 0.5));
	moon2p2.model = glm::translate(moon2p2.model, glm::vec3(10, 0, 10));
	initQuad(moon3p2);
	moon3p2.model = planet2.model;
	moon3p2.model = glm::scale(moon3p2.model, glm::vec3(0.5, 0.5, 0.5));
	moon3p2.model = glm::translate(moon3p2.model, glm::vec3(0, 0, -10));
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
	viewp.eye = { 0.0f,15.0f,30.0f };
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
	//Planet1
	planet.model = planet.model * glm::inverse(planet.model) * ymat * planet.model;
	//Moon1
	moon1p1.model =  glm::translate(glm::translate(moon1p1.model, glm::vec3(-10, 0, 0))*ymat,glm::vec3(10,0,0));
	moon1p1.model = moon1p1.model * glm::inverse(moon1p1.model) * ymat * moon1p1.model;
	//Moon2
	moon2p1.model = glm::translate(glm::translate(moon2p1.model, glm::vec3(10, 0, 0))*ymat, glm::vec3(-10, 0, 0));
	moon2p1.model = moon2p1.model * glm::inverse(moon2p1.model) * ymat * moon2p1.model;
	//Moon3
	moon3p1.model = glm::translate(glm::translate(moon3p1.model, glm::vec3(10, 0, 0))*ymat, glm::vec3(-10, 0, 0));
	moon3p1.model = moon3p1.model * glm::inverse(moon3p1.model) * ymat * moon3p1.model;
	//Moon4
	moon4p1.model = glm::translate(glm::translate(moon4p1.model, glm::vec3(-10, 0, 0))*ymat, glm::vec3(10, 0, 0));
	moon4p1.model = moon4p1.model * glm::inverse(moon4p1.model) * ymat * moon4p1.model;
	//Moon5
	moon5p1.model = glm::translate(glm::translate(moon5p1.model, glm::vec3(0, 0, 10))*ymat, glm::vec3(0, 0, -10));
	moon5p1.model = moon5p1.model * glm::inverse(moon5p1.model) * ymat * moon5p1.model;
	//Moon6
	moon6p1.model = glm::translate(glm::translate(moon6p1.model, glm::vec3(0, 0,-10))*ymat, glm::vec3(0, 0, 10));
	moon6p1.model = moon6p1.model * glm::inverse(moon6p1.model) * ymat * moon6p1.model;
	//Moon7
	moon7p1.model = glm::translate(glm::translate(moon7p1.model, glm::vec3(10, 0, 0))*ymat, glm::vec3(-10, 0, 0));
	moon7p1.model = moon7p1.model * glm::inverse(moon7p1.model) * ymat * moon7p1.model;
	//Moon8
	moon8p1.model = glm::translate(glm::translate(moon8p1.model, glm::vec3(-10, 0, 0))*ymat, glm::vec3(10, 0, 0));
	moon8p1.model = moon8p1.model * glm::inverse(moon8p1.model) * ymat * moon8p1.model;
	//Moon9
	moon9p1.model = glm::translate(glm::translate(moon9p1.model, glm::vec3(0, 0, 10))*ymat, glm::vec3(0, 0, -10));
	moon9p1.model = moon9p1.model * glm::inverse(moon9p1.model) * ymat * moon9p1.model;
	//Moon10
	moon10p1.model = glm::translate(glm::translate(moon10p1.model, glm::vec3(0, 0, -10))*ymat, glm::vec3(0, 0, 10));
	moon10p1.model = moon10p1.model * glm::inverse(moon10p1.model) * ymat * moon10p1.model;
	//Planet2
	planet2.model = planet2.model * glm::inverse(planet2.model) * ymat * planet2.model;
	//Moon1
	moon1p2.model = glm::translate(glm::translate(moon1p2.model, glm::vec3(10, 0, -10))*ymat, glm::vec3(-10, 0, 10));
	moon1p2.model = moon1p2.model * glm::inverse(moon1p2.model) * ymat * moon1p2.model;
	//Moon2
	moon2p2.model = glm::translate(glm::translate(moon2p2.model, glm::vec3(-10, 0, -10))*ymat, glm::vec3(10, 0, 10));
	moon2p2.model = moon2p2.model * glm::inverse(moon2p2.model) * ymat * moon2p2.model;
	//Moon3
	moon3p2.model = glm::translate(glm::translate(moon3p2.model, glm::vec3(0, 0, 10))*ymat, glm::vec3(0, 0, -10));
	moon3p2.model = moon3p2.model * glm::inverse(moon3p2.model) * ymat * moon3p2.model;

	initLines();
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
	// Shader program will be released upon program termination.
	releaseObject(quad);
}


void renderall()
{
	renderQuad(sun);
	renderQuad(planet);
	renderQuad(planet2);
	renderQuad(moon1p1);
	renderQuad(moon2p1);
	renderQuad(moon3p1);
	renderQuad(moon4p1);
	renderQuad(moon5p1);
	renderQuad(moon6p1);
	renderQuad(moon7p1);
	renderQuad(moon8p1);
	renderQuad(moon9p1);
	renderQuad(moon10p1);
	renderQuad(moon1p2);
	renderQuad(moon2p2);
	renderQuad(moon3p2);

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
   refreshMatrix(0.1f *(std::clock() - start)/50);
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
	case 'x':
		quad.model = quad.model * inverse;
		quad.model = quad.model * xmat;
		quad.model = quad.model * backup;
		break;
	case 'y':
		quad.model = quad.model * inverse;
		quad.model = quad.model * ymat;
		quad.model = quad.model * backup;
		break;
	case 'z':
		doSomething();
		break;
	case 'a':
		viewp.eye.z += 0.1;
		view = glm::lookAt(viewp.eye, viewp.center, viewp.up);
		break;
	case 's':
		viewp.eye.z -= 0.1;
		view = glm::lookAt(viewp.eye, viewp.center, viewp.up);
		break;
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
