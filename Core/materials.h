#ifndef MATERIALS_H
#define MATERIALS_H

#include "shaders.h"



class materialBase
{
public:
	materialBase(pxlShaderFunc _shader = whitePixShader): shader(_shader) {};
	~materialBase() {};

	pxlShaderFunc getShader() { return shader; }

protected:
	tNamedBuffers* textureMaps;
	tMaterialParam matParams[16];
	pxlShaderFunc shader;
};

class normalWhiteLight :public materialBase
{
public:
	normalWhiteLight() : materialBase(whiteLightShader) {};
	virtual ~normalWhiteLight() {};

};

class normalMaterial:public materialBase
{
public:
	normalMaterial() : materialBase(normalsPixShader) {};
	virtual ~normalMaterial() {};

};






#endif
