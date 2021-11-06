#ifndef WORLD_H
#define WORLD_H

#include "modelregistry.h"
#include "mesh.h"
#include "transformchain.h"
#include "camera.h"
#include "matrixmath.h"

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
		transform.translate(dx, dy, dz);
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

	tTransformMatrix* getRotM2Wmatrix()
	{
		return transform.getRotM2Wmatrix();
	}

	tTransformMatrix* getRotW2Mmatrix()
	{
		return transform.getRotW2Mmatrix();
	}

	void getLocation(float* _x)
	{
		transform.getLocation(_x);
	}

	void getAngles(float* _x)
	{
		transform.getAngles(_x);
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
	tCameraObject() : tTransformable() { construct(); };
	tCameraObject(std::string _sName, std::string _sCameraName, float _ratio) : tTransformable(_sName)
	{
		construct(_ratio);
		setName(_sName);
		
	};
	virtual ~tCameraObject() { delete W2Projection; };

	void construct(float _ratio = 1.f)
	{
		camera = std::make_shared<tCamera>(_ratio);
		W2Projection = new tTransformMatrix(W2ProjectionData);
	}

	void setCameraName(std::string _sName)
	{
		sCameraName = _sName;
	}

	std::string getCameraName()
	{
		return sCameraName;
	}

	tTransformMatrix* getW2Projection()
	{
		Multiply(*(camera->getProjectionmatrix()), *(transform.getW2Mmatrix()), W2Projection);
		return W2Projection;
	}

	void setCamera(float _ratio)
	{
		camera->setParams(_ratio);
	}

protected:
	std::string  sCameraName;
	std::shared_ptr<tCamera> camera;
	float W2ProjectionData[16];
	tTransformMatrix* W2Projection;

};


class tScene
{
public:
	tScene() { cam = new tCameraObject(); };
	~tScene() { delete cam; };

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

	void addBox(float _x = 0, float _y = 0, float _z = 0, float _xLen = 1, float _yLen = 1, float _zLen = 1,
				float xang=0, float yang=0, float zang=0 )
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
		objPtr->setAngles(xang, yang, zang);

		objPtr->UpdateTransforms();
	}

	void setCamera(float _ratio)
	{
		cam->setCamera(_ratio);
	}
	
	tCameraObject* getCameraPtr()
	{
		return cam;
	}

	std::unordered_map<int, tMeshObject*>* getObjectsPtr()
	{
		return &sceneObjects;
	}

	
	

private:

	tModelRegistry registry;
	std::unordered_map<int, tMeshObject*> sceneObjects ;
	tCameraObject* cam;

	int nextIdx = 0;
	
	float fTmp[24] = { 0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1 };
	int iTmp[36] = { 0,1,2 , 0,2,3,
							   0,4,5 , 0,5,1,
							   0,7,4 , 0,3,7,
							   1,5,6 , 1,6,2,
							   2,6,3 , 3,6,7,
							   4,7,6 , 4,6,5 };

};


#endif
