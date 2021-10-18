#ifndef MODELREGISTRY_H
#define MODELREGISTRY_H

#include <unordered_map>
#include <memory>
#include <string>
#include "mesh.h"

struct tRegRecord
{
	std::shared_ptr<tMesh> meshPtr;
	int refCount;
};

class tModelRegistry
{
public:
	tModelRegistry() {};
	~tModelRegistry() {};

	void registerMesh(const std::string& sName, std::shared_ptr<tMesh> _meshPtr)
	{
		if (!keyRegistered(sName))
		{
			tRegRecord rec;
			rec.refCount = 0;
			rec.meshPtr = _meshPtr;
			modelCollection[sName] = rec;
		}
		modelCollection[sName].refCount += 1;
	}

	void unregisterMesh(std::string& sName)
	{
		
		if (keyRegistered(sName))
			modelCollection[sName].refCount -= 1;
		if (modelCollection[sName].refCount <= 0)
			modelCollection.erase(sName);			
	}


	bool keyRegistered(const std::string& sName)
	{
		if (modelCollection.find(sName) == modelCollection.end())
			return false;
		return true;
	}

	std::shared_ptr<tMesh> getMesh(std::string key)
	{
		return modelCollection[key].meshPtr;
	}

private:
	std::unordered_map<std::string, tRegRecord> modelCollection;
};



#endif
