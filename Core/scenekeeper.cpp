#ifndef SCENEKEEPER_H
#define SCENEKEEPER_H

#include "vec3.h"
#include <vector>

class Triangle
{
public:
	Triangle(int idx0, int idx1, int idx2) 
	{ 
		vert_id[0] = idx0;
		vert_id[1] = idx1;
		vert_id[2] = idx2;
	};

	Triangle(int* ids)
	{
		vert_id[0] = ids[0];
		vert_id[1] = ids[1];
		vert_id[2] = ids[2];
	};

	~Triangle() {};

	void buildNormals(){}
private:
	int vert_id[3] = { 0 };
	float vert_normals[9];
};


class Mesh
{
public:
	Mesh() {};
	~Mesh() {};

	void addVertex(float x, float y, float z)
	{
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);
	}

	void setVerices(int vertnum, float* verts)
	{
		for (int i = 0; i < vertnum;i++)
		{
			addVertex(	verts[i * 3], 
						verts[i * 3 + 1], 
						verts[i * 3 + 2]);
		}
	}

	void addTriangle(int vertex0, int vertex1, int vertex2) { tris.push_back(Triangle(vertex0, vertex1, vertex2)); }
	void addTriangle(int* vertIds) { tris.push_back(Triangle(vertIds)); }

	void setTris(int trisnum, int* vertids)
	{
		for (int i = 0; i < trisnum;i++)
		{
			addTriangle(vertids[i * 3],
						vertids[i * 3 + 1],
						vertids[i * 3 + 2]);
		}
	}

private:
	std::vector<float> vertices;
	std::vector<Triangle> tris;
};





#endif