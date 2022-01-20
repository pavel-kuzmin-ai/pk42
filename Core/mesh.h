#ifndef MESH_H
#define MESH_H

#include "vec3.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

class tTriangle
{
public:
	tTriangle() {};
	tTriangle(int idx0, int idx1, int idx2)
	{
		vert_id[0] = idx0;
		vert_id[1] = idx1;
		vert_id[2] = idx2;
	};

	//tTriangle(int* ids)
	//{
	//	vert_id[0] = ids[0];
	//	vert_id[1] = ids[1];
	//	vert_id[2] = ids[2];
	//};

	~tTriangle() {};

	void buildNormals() {}
private:
	int vert_id[3] = { 0 };
	float vert_normals[9];
};


class tMesh
{
public:
	tMesh() {};
	~tMesh() {};


	void addPoint2Vect(float x, float y, float z, std::vector<float>& trgV)
	{
		trgV.push_back(x);
		trgV.push_back(y);
		trgV.push_back(z);
	}

	void setVerices(int vertnum, float* verts)
	{
		for (int i = 0; i < vertnum; i++)
		{
			addPoint2Vect(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2], vertices);
		}
	}

	void setNormals(int normNum, float* norms)
	{
		for (int i = 0; i < normNum; i++)
		{
			addPoint2Vect(norms[i * 3], norms[i * 3 + 1], norms[i * 3 + 2], normals);
		}
	}

	void addTriangleVerts(int vertex0, int vertex1, int vertex2) 
	{ 
		trisVerts.push_back(vertex0);
		trisVerts.push_back(vertex1);
		trisVerts.push_back(vertex2);
	}

	void addTriangleNormal(int vertex0, int vertex1, int vertex2)
	{
		trisNormals.push_back(vertex0);
		trisNormals.push_back(vertex1);
		trisNormals.push_back(vertex2);
	}
	//void addTriangle(int* vertIds) { tris.push_back(tTriangle(vertIds)); }

	void setTris(int trisnum, int* vertids)
	{
		for (int i = 0; i < trisnum; i++)
		{
			addTriangleVerts(vertids[i * 3],
							 vertids[i * 3 + 1],
							 vertids[i * 3 + 2]);
		}
	}

	std::vector<float>* getDataPtr()
	{
		return &vertices;
	}


	int vect2buf(std::vector<float>& inVect, float* buf)
	{
		for (size_t i = 0; i < inVect.size(); i++)
			buf[i] = (inVect)[i];
		return (int)(inVect.size());
	}
	int vertsToBuffer(float* buf)
	{
		return vect2buf(vertices, buf);
	}
	int normsToBuffer(float* buf)
	{
		return vect2buf(normals, buf);
	}

	int vect2buf(std::vector<int>& inVect, int* buf)
	{
		for (size_t i = 0; i < inVect.size(); i++)
			buf[i] = (inVect)[i];
		return (int)(inVect.size());
	}

	int trisVertsToBuffer(int* buf)
	{
		return vect2buf(trisVerts, buf);
	}

	int trisNormToBuffer(int* buf)
	{
		return vect2buf(trisNormals, buf);
	}


	void parseVertex(std::stringstream& ss)
	{
		float x, y, z;
		ss >> x;
		ss >> y;
		ss >> z;
		addPoint2Vect(x, y, z, vertices);
	}

	void parseNormal(std::stringstream& ss)
	{
		float x, y, z;
		ss >> x >> y >> z;
		addPoint2Vect(x, y, z, normals);
	}

	void parseFace(std::stringstream& ss)
	{
		//int x, y, z;

		int vrtx[3];
		int tex[3];
		int norm[3];

		int *trgarr[3];
		trgarr[0] = vrtx;
		trgarr[1] = tex;
		trgarr[2] = norm;

		
		const char delim = '/';

		std::string buf;

		for (int i = 0; i < 3; i++)
		{
			ss >> buf;


			size_t pos = buf.find(delim);
			int arridx = 0;

			while (pos != std::string::npos)
			{
				if (pos > 0)
				{
					std::stringstream tmpss;
					for (size_t j = 0; j < pos; j++) tmpss << buf[j];
					int val;
					tmpss >> val;
					trgarr[arridx][i] = val;
				}

				arridx++;
				buf.erase(0, pos + 1);
				pos = buf.find(delim);				
			}
			std::stringstream tmpss;
			tmpss << buf;
			int val;
			tmpss >> val;
			trgarr[arridx][i] = val;
		}
		
			

		addTriangleVerts(vrtx[0] - 1, vrtx[1] - 1, vrtx[2] - 1);
		addTriangleNormal(norm[0] - 1, norm[1] - 1, norm[2] - 1);
	}

	void loadObj(const std::string& path)
	{
		std::ifstream fs(path);

		std::string line;
		char flag;
		while (std::getline(fs, line))
		{
			std::stringstream ss(line);
			ss >> flag;
			
			if (flag == 'v')
			{
				if (line[1] == ' ') parseVertex(ss);
				if (line[1] == 'n') 
				{ 
					ss >> flag; 
					parseNormal(ss); 
				}
				if (line[1] == 't') { ss >> flag; };
			}
			if (flag == 'f') parseFace(ss);
		}	
		setName(path);
	}

	void setName(const std::string& name)
	{
		sName = name;
	}
		

protected:
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<int> trisVerts;
	std::vector<int> trisNormals;
	std::string sName;
private:
	
};


class tBox:private tMesh
{
public:
	tBox():tMesh()
	{
		float arr[24] = { 0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1 };
		for (int i = 0; i < 24; i++)
			vertices.push_back(arr[i]);

		int arr2[36] = { 0,1,2 , 0,2,3,
							   0,4,5 , 0,5,1,
							   0,4,7 , 0,7,3,
							   6,5,1 , 6,1,2,
							   6,2,3 , 6,3,7,
							   6,7,4 , 6,4,5 };
		
		for (int i = 0; i < 36; i++)
			trisVerts.push_back(arr2[i]);
	};
	virtual ~tBox() {};

private:

};



#endif
