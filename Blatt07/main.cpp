#include <iostream>
#include <vector>
#include <sstream>

#include <GL/glew.h>
//#include <GL/gl.h> // OpenGL header not necessary, included by GLEW
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "CubeTextured.h"
#include "ThingTextured.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
// GLUT window id/handle
int glutID = 0;

cg::ThingTextured* cubeTextured= nullptr;

glm::mat4x4 view;
glm::mat4x4 projection;

float zNear = 0.1f;
float zFar = 100.0f;

/*
Release resources on termination.
*/
void release()
{
	delete cubeTextured;
}

/*
 Initialization. Should return true if everything is ok and false if something went wrong.
 */
bool init()
{
	// OpenGL stuff. Set "background" color and enable depth testing.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Construct view matrix.
	glm::vec3 eye(0.0f, 0.0f, 8.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	view = glm::lookAt(eye, center, up);

	// Create cube objects.
	try
	{
		cubeTextured = new cg::ThingTextured("titan.jpg");//= new cg::CubeTextured("cocauv.jpg");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	std::cout << "Use x, y, z to rotate the currently selected cube" << std::endl;
	std::cout << "Use + and - to scale the currently selected cube" << std::endl;
	std::cout << "Use n to enable / disable normals for the currently selected cube" << std::endl;
	std::cout << "Use m to cycle the minification mode for texturing." << std::endl;
	std::cout << "Use M to cycle the magnification mode for texturing." << std::endl;

	return true;
}

/*
 Rendering.
 */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	cubeTextured ->render(view, projection);
}
/*
 Display callback.
 */
void glutDisplay ()
{
   render();
   glutSwapBuffers();
}

/*
 Resize callback.
 */
void glutResize (int width, int height)
{
	height = height < 1 ? 1 : height;
	glViewport(0, 0, width, height);

	// Construct projection matrix.
	projection = glm::perspective(45.0f, (float) width / height, zNear, zFar);
}

static TextureSampling minification  = Nearest;
static TextureSampling magnification = Nearest;
/*
 Callback for char input.
 */
void glutKeyboard (unsigned char keycode, int x, int y)
{
	auto& model = cubeTextured->getModel();

	switch (keycode)
	{
	case '+':
		model = glm::scale(model, glm::vec3(1.05f));
		break;
	case '-':
		model = glm::scale(model, glm::vec3(1.0f/1.05f));
		break;
	case 'x':
		model = glm::rotate(model, 0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
		break;
	case 'X':
		model = glm::rotate(model, -0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
		break;
	case 'y':
		model = glm::rotate(model, 0.1f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		break;
	case 'Y':
		model = glm::rotate(model, -0.1f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
		break;
	case 'z':
		model = glm::rotate(model, 0.1f, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		break;
	case 'Z':
		model = glm::rotate(model, -0.1f, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
		break;
	case 'm': {
	  minification = SamplingNext[minification];
	  cubeTextured->texture.setTexParameter(minification, magnification);
		std::ostringstream os; os << SamplingString[minification] << ", " << SamplingString[magnification] << std::flush;
		glutSetWindowTitle(os.str().c_str());
		break;
	}
	case 'M': {
	  magnification = SamplingNext[magnification];
	  cubeTextured->texture.setTexParameter(minification, magnification);
	  std::ostringstream os; os << SamplingString[minification] << ", " << SamplingString[magnification] << std::flush;
		glutSetWindowTitle(os.str().c_str());
		break;
	}
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

    glutCreateWindow("Aufgabenblatt 07");
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

    // init vertex-array-objects.
    {
       bool result = init();
       if (!result) {
	 release();
	 return -2;
       }
    }

    // GLUT: Loop until the user closes the window
    // rendering & event handling
    glutMainLoop ();

    // Cleanup everything on termination.
    release();
	
    return 0;
}
