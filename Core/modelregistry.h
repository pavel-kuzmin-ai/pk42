#ifndef MODELREGISTRY_H
#define MODELREGISTRY_H

#include <unordered_map>
#include <memory>
#include <string>
#include "mesh.h"

class tModelRegistry
{
public:
	tModelRegistry() {};
	~tModelRegistry() {};

	std::shared_ptr<tMesh> registerNewMesh(std::string sName)
	{
		std::shared_ptr<tMesh> new_ptr = std::make_shared<tMesh>();
		model_collection[sName] = new_ptr;
		return new_ptr;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<tMesh>> model_collection;
};



#endif
