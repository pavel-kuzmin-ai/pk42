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

	void registerMesh(std::string& sName)	
	{
		if (!keyRegistered(sName))
		{
			tRegRecord rec;
			rec.refCount = 0;
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


	bool keyRegistered(std::string& sName)
	{
		if (modelCollection.find(sName) == modelCollection.end())
			return false;
		return true;
	}

private:
	std::unordered_map<std::string, tRegRecord> modelCollection;
};



#endif
