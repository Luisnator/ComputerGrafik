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
#define M_PI acos(-1.0)

// Standard window width
const int WINDOW_WIDTH  = 640;
// Standard window height
const int WINDOW_HEIGHT = 480;
// GLUT window id/handle
int glutID = 0;

//circle edges
int edge = 5;

//circle radius
float radius = 1.f;

cg::GLSLProgram program;

glm::mat4x4 view;
glm::mat4x4 projection;

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
struct rgb
{
	double r;
	double g;
	double b;
};
struct hsv
{
	int h;
	double s;
	double v;
};
struct cmy
{
	double c;
	double m;
	double y;
};

Object triangle;
Object quad;
Object circle;
rgb quadcolor;

hsv rgbToHsv(rgb model)
{
	const double min = model.r < model.g ? (model.r < model.b ? model.r : model.b) : (model.g < model.b ? model.g : model.b);
	const double max = model.r > model.g ? (model.r > model.b ? model.r : model.b) : (model.g > model.b ? model.g : model.b);
	double H;
	double S;
	double V;
	double delta = max - min;
	if (max == min)
	{
		H = 0;
	}
	else if (max == model.r)
	{
		H = 60*(model.g - model.b) / delta;
	}
	else if (max == model.g)
	{
		H = 60*(2 + (model.b - model.r) / delta);
	}
	else if (max == model.b)
	{
		H = 60*(4 + (model.r - model.g) / delta);
	}
	if (max == 0)
	{
		S = 0;
	}
	else
	{
		S = delta / max;
	}
	V = max;
	hsv retVal;
	retVal.h = H;
	retVal.s = S;
	retVal.v = V;
	return retVal;
}
cmy rgbToCmy(rgb model)
{
	cmy retVal;
	retVal.c = 1 - model.r;
	retVal.m = 1 - model.g;
	retVal.y = 1 - model.b;
	return retVal;
}
rgb cmyToRgb(cmy model)
{
	rgb retVal;
	retVal.r = 1 - model.c;
	retVal.g = 1 - model.m;
	retVal.b = 1 - model.y;
	return retVal;
}
rgb hsvToRgb(hsv model)
{
	rgb retVal;
	int h = model.h / 60;
	double f = model.h / 60 - h;
	double p = model.v *(1 - model.s);
	double q = model.v *(1 - model.s*f);
	double t = model.v *(1 - model.s*(1 - f));
	switch (h) {
	case(0):
		retVal.r = model.v;
		retVal.g = t;
		retVal.b = p;
		return retVal;
	case(1):
		retVal.r = q;
		retVal.g = model.v;
		retVal.b = p;
		return retVal;
	case(2):
		retVal.r = p;
		retVal.g = model.v;
		retVal.b = t;
		return retVal;
	case(3):
		retVal.r = p;
		retVal.g = q;
		retVal.b = model.v;
		return retVal;
	case(4):
		retVal.r = t;
		retVal.g = p;
		retVal.b = model.v;
		return retVal;
	case(5):
		retVal.r = model.v;
		retVal.g = p;
		retVal.b = q;
		return retVal;
	case(6):
		retVal.r = model.v;
		retVal.g = t;
		retVal.b = p;
		return retVal;
	}
}
void menu()
{
	rgb temp1 = rgb();
	hsv temp2 = hsv();
	cmy temp3 = cmy();
	int answere;
	std::cout << "Welchen Farbtyp möchtest du eingeben" << std::endl;
	std::cout << "1)RGB" << std::endl;
	std::cout << "2)HSV" << std::endl;
	std::cout << "3)CMY" << std::endl;
	std::cin >> answere;
	switch (answere)
	{
	case(1):
	{
		double r;
		double g;
		double b;
		std::cout << "R:" << std::endl;
		std::cin >> r;
		std::cout << "G:" << std::endl;
		std::cin >> g;
		std::cout << "B:" << std::endl;
		std::cin >> b;
		temp1.r = r;
		temp1.g = g;
		temp1.b = b;
		temp2 = rgbToHsv(temp1);
		temp3 = rgbToCmy(temp1);
		quadcolor = temp1;
		std::cout << "RGB: " + std::to_string(temp1.r) + "," + std::to_string(temp1.g) + "," + std::to_string(temp1.b) << std::endl;
		std::cout << "HSV: " + std::to_string(temp2.h) + "°" + "," + std::to_string(temp2.s) + "," + std::to_string(temp2.v) << std::endl;
		std::cout << "CMY: " + std::to_string(temp3.c) + "," + std::to_string(temp3.m) + "," + std::to_string(temp3.y) << std::endl;
		break;
	}
	case(2):
	{
		int h;
		double s;
		double v;
		std::cout << "H:" << std::endl;
		std::cin >> h;
		std::cout << "S:" << std::endl;
		std::cin >> s;
		std::cout << "V:" << std::endl;
		std::cin >> v;
		temp2.h = h;
		temp2.s = s;
		temp2.v = v;
		temp1 = hsvToRgb(temp2);
		temp3 = rgbToCmy(temp1);
		quadcolor = temp1;
		std::cout << "RGB: " + std::to_string(temp1.r) + "," + std::to_string(temp1.g) + "," + std::to_string(temp1.b) << std::endl;
		std::cout << "HSV: " + std::to_string(temp2.h) + "°" + "," + std::to_string(temp2.s) + "," + std::to_string(temp2.v) << std::endl;
		std::cout << "CMY: " + std::to_string(temp3.c) + "," + std::to_string(temp3.m) + "," + std::to_string(temp3.y) << std::endl;
		break;
	}
	case(3):
	{
		double c;
		double m;
		double y;
		std::cout << "C:" << std::endl;
		std::cin >> c;
		std::cout << "M:" << std::endl;
		std::cin >> m;
		std::cout << "Y:" << std::endl;
		std::cin >> y;
		temp3.c = c;
		temp3.m = m;
		temp3.y = y;
		temp1 = cmyToRgb(temp3);
		temp2 = rgbToHsv(temp1);
		quadcolor = temp1;
		std::cout << "RGB: " + std::to_string(temp1.r) + "," + std::to_string(temp1.g) + "," + std::to_string(temp1.b) << std::endl;
		std::cout << "HSV: " + std::to_string(temp2.h) + "°" + "," + std::to_string(temp2.s) + "," + std::to_string(temp2.v) << std::endl;
		std::cout << "CMY: " + std::to_string(temp3.c) + "," + std::to_string(temp3.m) + "," + std::to_string(temp3.y) << std::endl;
		break;
	}
	}
}




void renderCircle()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * circle.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);
	


	// Bind vertex array object so we can render the 1 triangle.
	glBindVertexArray(circle.vao);
	glDrawElements(GL_TRIANGLES, edge*3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void renderTriangle()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * triangle.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);

	// Bind vertex array object so we can render the 1 triangle.
	glBindVertexArray(triangle.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void renderQuad()
{
	// Create mvp.
	glm::mat4x4 mvp = projection * view * quad.model;

	// Bind the shader program and set uniform(s).
	program.use();
	program.setUniform("mvp", mvp);
	
	// Bind vertex array object so we can render the 2 triangles.
	glBindVertexArray(quad.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void initTriangle()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	const std::vector<glm::vec3> vertices = { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f)};
	const std::vector<glm::vec3> colors = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
	const std::vector<GLushort> indices = { 0, 1, 2};

	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &triangle.vao);
	glBindVertexArray(triangle.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &triangle.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &triangle.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &triangle.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	triangle.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void initQuad()
{

	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	const std::vector<glm::vec3> vertices = { { -1.0f, 1.0f, 0.0f }, { -1.0, -1.0, 0.0 }, { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } };
	const std::vector<glm::vec3> colors = { { (float)quadcolor.r, (float)quadcolor.g, (float)quadcolor.b }, { (float)quadcolor.r, (float)quadcolor.g, (float)quadcolor.b }, { (float)quadcolor.r, (float)quadcolor.g, (float)quadcolor.b }, { (float)quadcolor.r, (float)quadcolor.g, (float)quadcolor.b } };
	const std::vector<GLushort> indices = { 0, 1, 2, 0, 2, 3 };


	GLuint programId = program.getHandle();
	GLuint pos;

	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &quad.vao);
	glBindVertexArray(quad.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &quad.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &quad.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &quad.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	quad.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.25f, 0.0f, 0.0f));
}

void initCircle()
{
	// Construct triangle. These vectors can go out of scope after we have send all data to the graphics card.
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<GLushort> indices;
	int count = 0;
	for (int i = 0; i < edge; i++)
	{
		//Punkte auf Kreis-Rand berechnen
		float angle = 2 * M_PI / (float)edge * (float)(i + 1);
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		glm::vec3 pos(x, y, 0.f);
		float angle1 = 2 * M_PI / (float)edge * (float)(i + 2);
		float x1 = radius * cos(angle1);
		float y1 = radius * sin(angle1);
		glm::vec3 pos2(x1, y1, 0.f);
		vertices.push_back(glm::vec3(0, 0, 0));
		vertices.push_back(pos);
		vertices.push_back(pos2);
		float seed = ((float)rand() / (RAND_MAX));
		float seed2 = ((float)rand() / (RAND_MAX));
		float seed3 = ((float)rand() / (RAND_MAX));
		colors.push_back(glm::vec3(seed,seed2,seed3));
		colors.push_back(glm::vec3(seed,seed2,seed3));
		colors.push_back(glm::vec3(seed,seed2,seed3));
		//Anordnung indices
		indices.push_back(count);
		count++;
		indices.push_back(count);
		count++;
		indices.push_back(count);
		count++;
	}

	GLuint programId = program.getHandle();
	GLuint pos;
	// Step 0: Create vertex array object.
	glGenVertexArrays(1, &circle.vao);
	glBindVertexArray(circle.vao);

	// Step 1: Create vertex buffer object for position attribute and bind it to the associated "shader attribute".
	glGenBuffers(1, &circle.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, circle.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Bind it to position.
	pos = glGetAttribLocation(programId, "position");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 2: Create vertex buffer object for color attribute and bind it to...
	glGenBuffers(1, &circle.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, circle.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

	// Bind it to color.
	pos = glGetAttribLocation(programId, "color");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Step 3: Create vertex buffer object for indices. No binding needed here.
	glGenBuffers(1, &circle.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

	// Unbind vertex array object (back to default).
	glBindVertexArray(0);

	// Modify model matrix.
	circle.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL: Set "background" color and enable depth testing.
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

	// Construct view matrix.
	glm::vec3 eye(0.0f, 0.0f, 4.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	view = glm::lookAt(eye, center, up);

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
	//initTriangle();
	initQuad();
	//initCircle();
	return true;
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
	releaseObject(triangle);
	releaseObject(quad);
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//renderTriangle();
	renderQuad();
	//renderCircle();
}

void glutDisplay ()
{
   GLCODE(render());
   glutSwapBuffers();
}

void refresh()
{
	//initTriangle();
	//renderTriangle();
	initQuad();
	renderQuad();
	//initCircle();
	//renderCircle();
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
	switch (keycode)
	{
	case 27: // ESC
	  glutDestroyWindow ( glutID );
	  return;
	  
	case '+':
		if(edge <= 30)
		edge += 1;
		refresh();
		break;
	case '-':
		if (edge > 3)
		{
			edge -= 1;
		}
		refresh();
		break;
	case 'x':
		// do something
		break;
	case 'y':
		// do something
		break;
	case 'z':
		// do something
		break;
	case 'q':
		if (radius <= 2.f)
		{
			radius += 0.1f;
		}
		refresh();
		break;
	case 'w':
		if (radius > 0.1f)
		{
			radius -= 0.1f;
		}
		refresh();
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	menu();
	// GLUT: Initialize freeglut library (window toolkit).
        glutInitWindowSize    (WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(40,40);
	glutInit(&argc, argv);

	// GLUT: Create a window and opengl context (version 4.3 core profile).
	glutInitContextVersion(4, 3);
	glutInitContextFlags  (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitDisplayMode   (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutCreateWindow("Aufgabenblatt 01");
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
