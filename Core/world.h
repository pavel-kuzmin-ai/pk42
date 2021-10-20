#ifndef WORLD_H
#define WORLD_H

#include "modelregistry.h"
#include "mesh.h"
#include "transformchain.h"

#include <unordered_map>
#include <string>
#include <memory>

class tTransformable
{
public:
	tTransformable() {};
	tTransformable(std::string _sName) { setName(_sName); };
	~tTransformable() {};

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

	void setName(std::string _sName)
	{
		sName = _sName;
	}

	std::string getName()
	{
		return sName;
	}


	void UpdateTransforms()
	{
		transform.UpdateTransforms();
	}

	tTransformMatrix* getM2Wmatrix()
	{
		return transform.getM2Wmatrix();
	}

	tTransformMatrix* getW2Mmatrix()
	{
		return transform.getW2Mmatrix();
	}
protected:
	std::string sName;
	tTransformChain transform;
};


class tMeshObject: public tTransformable
{
public:
	tMeshObject() : tTransformable() {};
	tMeshObject(std::string _sName, std::string _sMeshName, std::shared_ptr<tMesh> _geometry): tTransformable(_sName)
	{
		bindMesh(_geometry);
		setName(_sName);
	};
	virtual ~tMeshObject() {};

	void bindMesh(std::shared_ptr<tMesh> meshPtr) 
	{
		geometry = meshPtr;
	}

	std::shared_ptr<tMesh> getMeshPtr()
	{
		return geometry;
	}


	void setMeshName(std::string _sName)
	{
		sMeshName = _sName;
	}

	std::string getMeshName()
	{
		return sMeshName;
	}


protected:
	std::string  sMeshName;
	std::shared_ptr<tMesh> geometry;
};

class tCameraObject : public tTransformable
{
public:
	tCameraObject() : tTransformable() {};
	tCameraObject(std::string _sName, std::string _sCameraName, std::shared_ptr<tCamera> _camera) : tTransformable(_sName)
	{
		bindCamera(_camera);
		setName(_sName);
	};
	virtual ~tCameraObject() {};

	void bindCamera(std::shared_ptr<tCamera> camPtr)
	{
		camera = camPtr;
	}

	std::shared_ptr<tCamera> getCamPtr()
	{
		return camera;
	}


	void setCameraName(std::string _sName)
	{
		sCameraName = _sName;
	}

	std::string getCameraName()
	{
		return sCameraName;
	}


protected:
	std::string  sCameraName;
	std::shared_ptr<tCamera> camera;
};


class tScene
{
public:
	tScene() {};
	~tScene() {};

	int addObject(std::string meshName, std::string modelName, std::shared_ptr<tMesh> geometry)
	{
		registry.registerMesh(meshName, geometry);
		tMeshObject* obj = new tMeshObject(modelName, meshName, geometry);
		sceneObjects[nextIdx] = obj;
		int myidx = nextIdx;
		nextIdx++;
		return myidx;
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
			geometry = registry.getMesh("box");
		}			
		else
		{
			geometry = std::make_shared<tMesh>();
			geometry->setVerices(8, fTmp);
			geometry->setTris(12, iTmp);
		}
		 
		int newidx = addObject("box", "boxModel", geometry);
		tMeshObject* objPtr = sceneObjects[newidx];
		objPtr->setLocation(_x, _y, _z);
		objPtr->setScale(_xLen, _yLen, _zLen);
		objPtr->UpdateTransforms();
	}

	std::unordered_map<int, tMeshObject*>* getObjectsPtr()
	{
		return &sceneObjects;
	}
	

private:

	tModelRegistry registry;
	std::unordered_map<int, tMeshObject*> sceneObjects ;
	int nextIdx = 0;
	
	float fTmp[24] = { 0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1 };
	int iTmp[36] = { 0,1,2 , 0,2,3,
							   0,4,5 , 0,5,1,
							   0,4,7 , 0,7,3,
							   6,5,1 , 6,1,2,
							   6,2,3 , 6,3,7,
							   6,7,4 , 6,4,5 };

};


#endif
