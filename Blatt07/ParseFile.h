#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <glm/glm.hpp>
struct Halfedge;
struct Face;
struct Vertex;
struct Boundingbox;

typedef struct Halfedge
{
	Vertex *vert;
	Halfedge* pair;
	Face* face;
	Halfedge* next;
}Halfedge;
typedef struct Face {
	Halfedge* edge;
	glm::vec3 normal;
}Face;
typedef struct Vertex
{
	float x;
	float y;
	float z;
	glm::vec3 normal;
	Halfedge* edge;
}Vertex;

typedef struct Boundingbox
{
	std::vector<std::vector<float>> vertices;
	std::vector<short> indices;
}Boundingbox;

using namespace std;
class ParseFile
{
public:
	//Functions
	ParseFile();
	~ParseFile();
	bool parseFile(std::string path);
	void writeToTest();
	vector<Face*> getHalfEdge();
	void checkConsistence();
	//Attributes
	Boundingbox boundingbox;
	vector<vector<float> > vertices;
	vector<vector<float> > indices;
	vector<glm::vec3> normals;
	vector<glm::vec2> texturcoord;
	vector<vector<float>> inditex;
	vector<vector<float>> indinorm;
	vector<vector<Halfedge*>> help;
	vector<Face*> facelist;
	vector<Halfedge*> halfedgelist;
	vector<Vertex*> vertexlist;
private:
	void createBoundingbox(float maxX, float minX,float maxY,float minY,float maxZ, float minZ);
};

