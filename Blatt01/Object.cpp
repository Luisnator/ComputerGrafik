#include "Object.h"

Object::Object()
{

}
glm::mat4x4* Object::getModel()
{
	return &model;
}
GLuint* Object::getVao()
{
	return &vao;
}
GLuint* Object::getColorBuffer()
{
	return &colorBuffer;
}
GLuint* Object::getIndexBuffer()
{
	return &indexBuffer;
}

void Object::attach(Object & attach)
{
	this->at = &attach;
}

void Object::notify()
{
}

void Object::setInheritRotation(bool boo)
{
	inherit = boo;
}

void Object::inheritmove(glm::mat4x4 last, glm::mat4x4 now)
{
}

void Object::rotatelocal(glm::mat4x4 mod, float angle, glm::vec3 dir)
{
	lastmodel = model;
	glm::rotate(mod, angle, dir);
	notify();
}

Object::~Object()
{

}
