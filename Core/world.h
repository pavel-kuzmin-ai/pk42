#ifndef WORLD_H
#define WORLD_H

#include "modelregistry.h"
#include "mesh.h"
#include "transformchain.h"

#include <unordered_map>
#include <string>
#include <memory>

class tMeshObject
{
public:
	tMeshObject() {};
	tMeshObject(std::string _sName, std::string _sMeshName, std::shared_ptr<tMesh> geometry)
	{
		bindMesh(geometry);
		setName(_sName);
	};
	~tMeshObject() {};

	void setLocation(float _x, float _y, float _z)
	{
		transform.setLocation(_x, _y, _z);
	}

	void translate(float dx, float dy, float dz)
	{
		transform.setLocation(dx, dy, dz);
	}

	void setScale(float _x, float _y, float _z)
	{
		transform.setScale(_x, _y, _z);
	}

	void ScaleBy(float ScaleX, float ScaleY, float ScaleZ)
	{
		transform.ScaleBy(ScaleX, ScaleY, ScaleZ);
	}

	void setAngles(float rotX, float rotY, float rotZ)
	{
		transform.setAngles(rotX, rotY, rotZ);
	}

	void rotateBy(float rotX, float rotY, float rotZ)
	{
		transform.rotateBy(rotX, rotY, rotZ);
	}

	void bindMesh(std::shared_ptr<tMesh> meshPtr) 
	{
		geometry = meshPtr;
	}

	std::shared_ptr<tMesh> getMeshPtr()
	{
		return geometry;
	}

	void setName(std::string _sName)
	{
		sName = _sName;
	}

	std::string getName()
	{
		return sName;
	}

	void setMeshName(std::string _sName)
	{
		sMeshName = _sName;
	}

	std::string getMeshName()
	{
		return sMeshName;
	}



	void UpdateTransforms()
	{
		transform.UpdateTransforms();
	}
protected:
	std::string sName, sMeshName;
	std::shared_ptr<tMesh> geometry;
	tTransformChain transform;
};

class tBoxObj:private tMeshObject
{
public:
	tBoxObj(float _x = 0, float _y = 0, float _z = 0, float _xLen = 1, float _yLen = 1, float _zLen = 1)
	{
		geometry = std::make_shared<tBox>();
		setName("box");
		setLocation(_x, _y, _z);
		setScale(_xLen, _yLen, _zLen);
		UpdateTransforms();
	};
	virtual ~tBoxObj() {};
};


class tScene
{
public:
	tScene() {};
	~tScene() {};

	void addObject(std::string meshName, std::string modelName, std::shared_ptr<tMesh> geometry)
	{
		registry.registerMesh(meshName, geometry);
		tMeshObject* obj = new tMeshObject(modelName, meshName, geometry);
		sceneObjects[nextIdx] = obj;
		nextIdx++;
	}

	void removeObject(int idx)
	{

		tMeshObject* ptr = sceneObjects[idx];
		std::string meshName = ptr->getMeshName();
		sceneObjects.erase(idx); 
		delete ptr;
		
		registry.unregisterMesh(meshName);
	}

	void addBox(float _x = 0, float _y = 0, float _z = 0, float _xLen = 1, float _yLen = 1, float _zLen = 1)
	{
		std::shared_ptr<tMesh> geometry;
		if (registry.keyRegistered("box"))
		{
			geometry = registry["box"].meshPtr;
		}			
		else
		{
			geometry = std::make_shared<tBox>();
		}
		 
		addObject("box", "boxModel", geometry);
	}

	
	

private:

	tModelRegistry registry;
	std::unordered_map<int, tMeshObject*> sceneObjects ;
	int nextIdx = 0;
	


};


#endif
