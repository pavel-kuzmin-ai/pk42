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

	bool assetRegistered(const std::string& name)
	{
		if (assetUsageCounter.find(name) == assetUsageCounter.end())
			return false;
		return true;
	}

	void registerAsset(const std::string& name)
	{
		if (!assetRegistered(name)) assetUsageCounter[name] = 0;
		assetUsageCounter[name]++;
	}

	void unregisterAsset(const std::string& name)
	{
		if (assetRegistered(name)) assetUsageCounter[name]--;
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
	
	std::shared_ptr<tMesh> GetOrLoadMesh(const std::string& name, const std::string& path)
	//tMesh* GetOrLoadMesh(const std::string& name, const std::string& path)
	{
		std::shared_ptr<tMesh> ptr;
		//tMesh* ptr;
		if (!assetRegistered(name))
		{
			ptr = std::make_shared<tMesh>();
			//ptr = new tMesh;
			ptr->loadObj(path);
			meshRegistry[name] = ptr;
		}
		else
		{
			ptr = meshRegistry[name];
		}
		registerAsset(name);
		return ptr;
	}



private:
	std::unordered_map<std::string, int> assetUsageCounter;
	std::unordered_map<std::string, std::shared_ptr<tMesh>> meshRegistry;
	//std::unordered_map<std::string, tMesh*> meshRegistry;

};


#endif // !ASSETLOADER_H

