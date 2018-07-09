#include "ParseFile.h"
#include <fstream>
#include <sstream>
#include <limits>
using namespace std;

ParseFile::ParseFile()
{
}


ParseFile::~ParseFile()
{
}

bool ParseFile::parseFile(std::string path)
{
	vertices.clear();
	indices.clear();
	normals.clear();
	ifstream inputstream;
	inputstream.open(path);
	if (inputstream.is_open())
	{
		string type;
		
		string line;
		float maxX = numeric_limits<float>::min(), maxY = numeric_limits<float>::min(), maxZ = numeric_limits<float>::min();
		float minX = numeric_limits<float>::max(), minY = numeric_limits<float>::max(), minZ = numeric_limits<float>::max();
		while (getline(inputstream, line))
		{
			type = "";
			vector<string> values2;
			vector<float> values;
			istringstream in(line);
			in >> type;
			while (in.rdbuf()->in_avail() != 0)
			{
				string temp;
				in >> temp;
				values2.push_back(temp);
			}
			if (type == "v")
			{
				if (maxX < strtof(values2[0].c_str(), 0))maxX = strtof(values2[0].c_str(), 0);
				if (minX > strtof(values2[0].c_str(), 0))minX = strtof(values2[0].c_str(), 0);
				if (maxY < strtof(values2[1].c_str(), 0))maxY = strtof(values2[1].c_str(), 0);
				if (minY > strtof(values2[1].c_str(), 0))minY = strtof(values2[1].c_str(), 0);
				if (maxZ < strtof(values2[2].c_str(), 0))maxZ = strtof(values2[2].c_str(), 0);
				if (minZ > strtof(values2[2].c_str(), 0))minZ = strtof(values2[2].c_str(), 0);
				for (int i = 0; i < values2.size(); i++)
				{
					values.push_back(strtof(values2[i].c_str(), 0));
				}
				vertices.push_back(values);
			}
			else if (type == "f")
			{
				float first = strtof(values2[0].substr(0,values2[0].find("/")).c_str(),0);
				for (int i = 0; i < values2.size()-2; i++)
				{
					vector<float> indi;
					indi.push_back(first);
					indi.push_back(strtof(values2[i+1].substr(0, values2[i+1].find("/")).c_str(), 0));
					indi.push_back(strtof(values2[i+2].substr(0, values2[i+2].find("/")).c_str(), 0));
					indices.push_back(indi);
				}
			}
		}

		createBoundingbox(maxX, minX, maxY, minY, maxZ, minZ);
		cout << "\n\n\n";
	}
	else
	{
		cout << "File not found";
		return false;
	}
	inputstream.close();
	return true;
}

void ParseFile::writeToTest()
{
	ofstream writer;
	writer.open("Test.obj");
	if (writer.is_open())
	{
		//Vertices
		for (int i = 0; i < vertices.size(); i++)
		{
			writer << "v ";
			for (int j = 0; j < vertices[i].size(); j++)
			{
				writer << vertices[i][j] << " ";
			}
			writer << "\n";
		}
		//Indices
		for (int i = 0; i < indices.size(); i++)
		{
			writer << "f ";
			for (int j = 0; j < indices[i].size(); j++)
			{
				writer << indices[i][j] << " ";
			}
			writer << "\n";
		}
		writer.close();
		
	}
}

vector<Face*> ParseFile::getHalfEdge()
{
	for (int i = 0; i < indices.size(); i++)
	{
		Face* f = (Face*)malloc(sizeof(Face));
		//Bind Vertex
		Halfedge* h1 = (Halfedge*)malloc(sizeof(Halfedge));
		Vertex* v1 = (Vertex*)malloc(sizeof(Vertex));
		v1->x = vertices[indices[i][0] - 1][0];
		v1->y = vertices[indices[i][0] - 1][1];
		v1->z = vertices[indices[i][0] - 1][2];
		v1->edge = h1;
		h1->vert = v1;
		Halfedge* h2 = (Halfedge*)malloc(sizeof(Halfedge));
		Vertex* v2 = (Vertex*)malloc(sizeof(Vertex));
		v2->x = vertices[indices[i][1] - 1][0];
		v2->y = vertices[indices[i][1] - 1][1];
		v2->z = vertices[indices[i][1] - 1][2];
		v2->edge = h2;
		h2->vert = v2;
		Halfedge* h3 = (Halfedge*)malloc(sizeof(Halfedge));
		Vertex* v3 = (Vertex*)malloc(sizeof(Vertex));
		v3->x = vertices[indices[i][2] - 1][0];
		v3->y = vertices[indices[i][2] - 1][1];
		v3->z = vertices[indices[i][2] - 1][2];
		v3->edge = h3;
		h3->vert = v3;
		//Face startedge
		f->edge = h1;
		//Bind Next
		h1->next = h2;
		h2->next = h3;
		h3->next = h1;
		//Bind Face
		h1->face = f;
		h2->face = f;
		h3->face = f;
		//Calculate Normal for Face
		glm::vec3 vec1 = {v2->x - v1->x, v2->y - v1->y, v2->z - v1->z};
		glm::vec3 vec2 = {v3->x - v1->x, v3->y - v1->y, v3->z - v1->z};
		glm::vec3 normal = {(vec1.y*vec2.z)-(vec1.z*vec2.y),(vec1.z*vec2.x)-(vec1.x*vec2.z),(vec1.x*vec2.y)-(vec1.y*vec2.x)};
		normal = glm::normalize(normal);
		f->normal = normal;
		//Add to list
		facelist.push_back(f);
		halfedgelist.push_back(h1);
		halfedgelist.push_back(h2);
		halfedgelist.push_back(h3);
		vertexlist.push_back(v1);
		vertexlist.push_back(v2);
		vertexlist.push_back(v3);
		help.resize(vertices.size());
		help[indices[i][0] - 1].push_back(h1);
		help[indices[i][1] - 1].push_back(h2);
		help[indices[i][2] - 1].push_back(h3);
	}
	//Match pairs
	for (int i = 0; i < vertices.size(); i++)
	{
		vector<Halfedge*> list = help[i];
		for (int j = 0; j < list.size(); j++)
		{
				for (int l = 0; l < list.size(); l++)
				{
					if (list[j]->next->vert->x == list[l]->next->next->vert->x && list[j]->next->vert->y == list[l]->next->next->vert->y && list[j]->next->vert->z == list[l]->next->next->vert->z)
					{
						list[j]->pair = list[l]->next->next;
					}
				}
		}

	}
	//Calculate Normal for Vertex
	for (int i = 0; i < vertexlist.size(); i++)
	{
		Vertex *v1 = vertexlist[i];
		Halfedge *h1 = v1->edge;
		glm::vec3 normal = { 0,0,0 };
		do
		{
			normal += h1->face->normal;
			h1 = h1->pair->next;
		} while (h1->vert != v1);
		normal = glm::normalize(normal);
		v1->normal = normal;
	}
	return facelist;
}

void ParseFile::checkConsistence()
{
	map<int,int> vertexvalence;
	map<int,int> facevalence;
	//Check Halfedge-Pairs
	for (int i = 0; i < halfedgelist.size(); i++)
	{
		Halfedge *h1 = halfedgelist[i];
		Halfedge *h2 = h1->pair;
		if (h1 == h2->pair)
		{
			cout << "Halfedge " << i << ". : ist konsistent \n";
		}
		else
		{
			cout << "Halfedge " << i << ". : ist nicht konsistent \n";
		}
	}
	cout << "\n";
	//Check Faces
	for (int i = 0; i < facelist.size(); i++)
	{
		int fvalance = 0;
		Face *f1 = facelist[i];
		Halfedge *h1 = f1->edge;
		Halfedge *temp = h1;
		short consis = 1;
		if (h1->face != f1) consis = 0;
		do
		{
			fvalance++;
			temp = temp->next;
			if (temp->face != f1) consis = 0;
		} while (temp != h1);
		if (consis == 1)
		{
			cout << "Face " << i << ". : ist konsistent \n";
		}
		else
		{
			cout << "Face " << i << ". : ist nicht konsistent \n";
		}
		facevalence[fvalance]++;
	}
	cout << "\n";
	//Check Vertex
	for (int i = 0; i < vertexlist.size(); i++)
	{
		int vvalence = 0;
		Vertex *v1 = vertexlist[i];
		Halfedge *h1 = v1->edge;
		do
		{
			vvalence++;
			h1 = h1->pair->next;
		} while (h1->vert != v1);
		cout << "Vertex " << i << ". : ist kosistent \n";
		vertexvalence[vvalence]++;
	}
	cout << "\n";
	cout << "Statistiken: \n";
	//Stats
	//Facevalance
	cout << "Es gibt " << facelist.size() << " Faces\n";
	for (auto it = facevalence.begin(); it != facevalence.end(); ++it)
	{
		cout << "Valenz " << it->first << " haben " << it->second << " Faces \n";
	}
	//Vertexvalence
	cout << "\n";
	cout << "Es gibt " << vertexlist.size() << " Vertices\n";
	for (auto it = vertexvalence.begin(); it != vertexvalence.end(); ++it)
	{
		cout << "Valenz " << it->first << " haben " << it->second << " Vertices \n";
	}
	cout << "\n";
	//Halfedge
	cout << "Es gibt " << halfedgelist.size() << " Halfedges\n";
	cout << "\n\n\n";
}

void ParseFile::createBoundingbox(float maxX, float minX, float maxY, float minY, float maxZ, float minZ)
{
	vector<float> minmaxmin = {minX,maxY,minZ};
	vector<float> maxmaxmin = {maxX,maxY,minZ};
	vector<float> maxmaxmax = {maxX,maxY,maxZ};
	vector<float> minmaxmax = {minX,maxY,maxZ};
	vector<float> minminmin = {minX,minY,minZ};
	vector<float> maxminmin = {maxX,minY,minZ};
	vector<float> maxminmax = {maxX,minY,maxZ};
	vector<float> minminmax = {minX,minY,maxZ};
	boundingbox.vertices.push_back(minmaxmin);
	boundingbox.vertices.push_back(maxmaxmin);
	boundingbox.vertices.push_back(maxmaxmax);
	boundingbox.vertices.push_back(minmaxmax);
	boundingbox.vertices.push_back(minminmin);
	boundingbox.vertices.push_back(maxminmin);
	boundingbox.vertices.push_back(maxminmax);
	boundingbox.vertices.push_back(minminmax);
	boundingbox.indices = { 0,1, 0,4, 0,3, 1,2, 1,5, 2,3, 2,6, 3,7, 4,7, 4,5, 5,6, 6,7 };
}
