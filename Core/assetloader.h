#ifndef ASSETLOADER_H
#define ASSETLOADER_H

//#include "modelregistry.h"
#include <unordered_map>
#include <memory>
#include <string>
#include "mesh.h"

class tAsset
{
public:
	tAsset() {};
	~tAsset() {};

private:

	std::string sName;
	std::string sPath;
};


class tAssetLoader
{
public:
	tAssetLoader() {}
	~tAssetLoader() {}

	bool assetRegistered(const std::string& sName)
	{
		if (assetUsageCounter.find(sName) == assetUsageCounter.end())
			return false;
		return true;
	}

	void registerAsset(const std::string& path) 
	{
		if (!assetRegistered(path)) assetUsageCounter[path] = 0;
		assetUsageCounter[path]++;
	}

	void unregisterAsset(const std::string& path) 
	{
		if (assetRegistered(path)) assetUsageCounter[path]--;
	}

	void vacuum() 
	{
		for (auto iter : assetUsageCounter)
		{
			if (iter.second <= 0)
			{
				assetUsageCounter.erase(iter.first);
				meshRegistry.erase(iter.first);
			}
		}
	}
	
	std::shared_ptr<tMesh> GetOrLoadMesh(const std::string& path)
	{
		std::shared_ptr<tMesh> ptr;
		if (!assetRegistered(path))
		{
			ptr = std::make_shared<tMesh>();
			ptr->loadObj(path);
		}
		else
		{
			ptr = meshRegistry[path];
		}
		registerAsset(path);
		return ptr;
	}



private:
	std::unordered_map<std::string, int> assetUsageCounter;
	std::unordered_map<std::string, std::shared_ptr<tMesh>> meshRegistry;

};


#endif // !ASSETLOADER_H

