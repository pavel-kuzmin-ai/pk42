#ifndef WORLD_H
#define WORLD_H

//#include "modelregistry.h"
#include "mesh.h"
#include "transformchain.h"
#include "camera.h"
#include "matrixmath.h"
#include "assetloader.h"

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
	tMeshObject(std::string _sName, std::string _sMeshName, std::shared_ptr<tMesh> _geometry): 
		tTransformable(_sName), sMeshName(_sMeshName)
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

	bool bIsVisible() { return bVisible; }
	void setVisible(bool visibilityStatus) { bVisible = visibilityStatus; }


protected:
	std::string  sMeshName;
	std::shared_ptr<tMesh> geometry;
	bool bVisible = false;
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

	
	
	int addMeshObject(std::string MeshPath)
	{
		tMeshObject* obj = new tMeshObject(MeshPath, MeshPath, assetLoader.GetOrLoadMesh(MeshPath, MeshPath));
		int myidx = nextIdx;
		sceneObjects[myidx] = obj;
		nextIdx++;
		return myidx;
	}

	void removeObject(int idx)
	{

		tMeshObject* ptr = sceneObjects[idx];
		std::string meshName = ptr->getMeshName();
		sceneObjects.erase(idx);
		delete ptr;

		assetLoader.unregisterAsset(meshName);
		assetLoader.vacuum();
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

	void setVisibilityStatus(int idx, bool status)
	{
		if (sceneObjects.find(idx) != sceneObjects.end())
		{
			sceneObjects[idx]->setVisible(status);
		}			
	}

	void spawnObject(int idx,
		float x = 0.f, float y = 0.f, float z = 0.f,
		float rotx = 0.f, float roty = 0.f, float rotz = 0.f,
		float scx = 1.f, float scy = 1.f, float scz = 1.f)
	{
		if (sceneObjects.find(idx) != sceneObjects.end())
		{
			tMeshObject* obj = sceneObjects[idx];
			obj->setLocation(x, y, z);
			obj->setAngles(rotx, roty, rotz);
			obj->setScale(scx, scy, scz);
			obj->UpdateTransforms();
			obj->setVisible(true);
		}
	}

	
	

private:
	tAssetLoader assetLoader;
	std::unordered_map<int, tMeshObject*> sceneObjects ;
	tCameraObject* cam;

	int nextIdx = 0;
	

};


#endif
