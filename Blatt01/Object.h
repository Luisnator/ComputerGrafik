
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
class Object
{
public:
	Object();
	glm::mat4x4* getModel();
	GLuint* getVao();
	GLuint* getColorBuffer();
	GLuint* getIndexBuffer();
	void attach(Object &attach);
	void notify();
	void setInheritRotation(bool boo);
	void inheritmove(glm::mat4x4 last, glm::mat4x4 now);
	void rotatelocal(glm::mat4x4 mod,float angle,glm::vec3 dir);
	~Object();
private:
	//Programmstuff
	glm::mat4x4 view;
	glm::mat4x4 projection;
	cg::GLSLProgram program;
	//Modelstuff
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;
	glm::mat4x4 lastmodel;
	glm::mat4x4 model;
	Object* at;
	//otherStuff
	std::vector<Object> attachlist;
	bool inherit;
	int m_eineVariable;
};
