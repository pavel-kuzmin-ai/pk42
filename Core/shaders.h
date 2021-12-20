#ifndef SHADERS_H
#define SHADERS_H

#include "matrixmath.h"
#include "glcommons.h"

//geometry shaders
//----------------------------------------------------------------

float tmpCoordsData[4] = { 1., 1., 1., 1. };
tMatrix* tmpCoords = new tMatrix(4, 1, tmpCoordsData);

float tmpNormalData[4] = { 1., 1., 1., 1. };
tMatrix* tmpNormal = new tMatrix(4, 1, tmpNormalData);

float tmpOutputData[4] = { 1., 1., 1., 1. };
tMatrix* tmpOutput = new tMatrix(4, 1, tmpOutputData);


void moveNormalsToLocations(tPixelData& in, tPixelData& out)
{
	out.normal[0] = in.xyz[0] + in.normal[0];
	out.normal[1] = in.xyz[1] + in.normal[1];
	out.normal[2] = in.xyz[2] + in.normal[2];

}

void restoreNormals(tPixelData& in, tPixelData& out)
{
	out.normal[0] = in.normal[0] - in.xyz[0];
	out.normal[1] = in.normal[1] - in.xyz[1];
	out.normal[2] = in.normal[2] - in.xyz[2];
	
}

void transformVerts(tPixelData& in, tMatrix* M, tPixelData& out)
{
	arr2Matrix(in.xyz, *tmpCoords, 3, 0);

	Multiply(*M, *tmpCoords, tmpOutput);
	out.xyz[0] = tmpOutput->getValue(0, 0);
	out.xyz[1] = tmpOutput->getValue(1, 0);
	out.xyz[2] = tmpOutput->getValue(2, 0);
}

void transformNormals(tPixelData& in, tMatrix* M, tPixelData& out)
{
	arr2Matrix(in.normal, *tmpCoords, 3, 0);

	Multiply(*M, *tmpCoords, tmpOutput);
	out.normal[0] = tmpOutput->getValue(0, 0);
	out.normal[1] = tmpOutput->getValue(1, 0);
	out.normal[2] = tmpOutput->getValue(2, 0);

}

void calcViewDirection(tPixelData& in, tPixelData& out, bool perspective = true)
{
	if (perspective)
	{
		float overlen = 1.f / euclideanDist(in.xyz);
		out.viewDirection[0] = in.xyzCam[0] * overlen;
		out.viewDirection[1] = in.xyzCam[1] * overlen;
		out.viewDirection[2] = in.xyzCam[2] * overlen;
	}
	else
		out.viewDirection[0] = 0.f;
		out.viewDirection[1] = 0.f;
		out.viewDirection[2] = -1.f;		
}


void geometryShader(tPixelData& in, tPixelData& out, tMatrix* M2C, tMatrix* C2V)
{
	moveNormalsToLocations(in, out);
	transformVerts(in, M2C, out);
	std::memcpy(out.xyzCam, out.xyz, 3 * sizeof(float));
	transformNormals(out, M2C, out);
	restoreNormals(out, out);
	calcViewDirection(out, out, false);
	transformVerts(out, C2V, out);
}

void faceNormalRotateShader(tPixelData& in, tPixelData& out, tMatrix* M2C, tMatrix* C2V)
{
	moveNormalsToLocations(in, out);
	transformVerts(in, M2C, out);
	transformNormals(out, M2C, out);
	restoreNormals(out, out);
	calcViewDirection(out, out, false);
}


void geometryShaderCut(tPixelData& in, tPixelData& out, tMatrix* M2C, tMatrix* C2V)
{
	transformVerts(in, M2C, out);
	transformVerts(out, C2V, out);
}

/*
tPixelData geometryShader(tPixelData& v, tMatrix* M2C, tMatrix* C2V)
{
	float corrNormal[3];
	corrNormal[0] = v.xyz[0] + v.normal[0];
	corrNormal[1] = v.xyz[1] + v.normal[1];
	corrNormal[2] = v.xyz[2] + v.normal[2];

	tPixelData out;
	arr2Matrix(v.xyz, *tmpCoords, 3, 0);

	Multiply(*M2C, *tmpCoords, tmpOutput);

	out.xyz[0] = tmpOutput->getValue(0, 0);
	out.xyz[1] = tmpOutput->getValue(1, 0);
	out.xyz[2] = tmpOutput->getValue(2, 0);


	
	arr2Matrix(corrNormal, *tmpNormal, 3, 0);
	Multiply(*M2C, *tmpNormal, tmpOutput);
	
	out.normal[0] = tmpOutput->getValue(0, 0) - out.xyz[0];
	out.normal[1] = tmpOutput->getValue(1, 0) - out.xyz[1];
	out.normal[2] = tmpOutput->getValue(2, 0) - out.xyz[2];

	
	arr2Matrix(out.xyz, *tmpCoords, 3, 0);

	Multiply(*C2V, *tmpCoords, tmpOutput);
	out.xyz[0] = tmpOutput->getValue(0, 0);
	out.xyz[1] = tmpOutput->getValue(1, 0);
	out.xyz[2] = tmpOutput->getValue(2, 0);

	arr2Matrix(out.normal, *tmpCoords, 3, 0);

	Multiply(*C2V, *tmpCoords, tmpOutput);
	out.lineOfSightNormProj = tmpOutput->getValue(2, 0) + 1.0f;
	return out;
}*/

//pixel shaders
//----------------------------------------------------------------


struct tMaterialParam
{
	fBuffer* map;
	bool isMapSet = false;
	float* fValue;
};

typedef void(*pxlShaderFunc)(tPixelData& currVrtx, tMaterialParam* materialParamList, tColorData& out);

//struct tShaderParams
//{
//	float k_s, k_d, k_a, alpha;
//};

void whitePixShader(tPixelData& currVrtx, tMaterialParam* materialParamList, tColorData& out)
{
	out.rgba[0] = 1.f;
	out.rgba[1] = 1.f;
	out.rgba[2] = 1.f;
	out.rgba[3] = 1.f;
}

void whiteLightShader(tPixelData& currVrtx, tMaterialParam* materialParamList, tColorData& out)
{
	float mult = std::abs(dot(currVrtx.viewDirection, currVrtx.normal));
	out.rgba[0] = 1.f * mult;
	out.rgba[1] = 1.f * mult;
	out.rgba[2] = 1.f * mult;
	out.rgba[3] = 1.f;
}

enum MATERIALMAPS
{
	MAP_NORMAL = 0
};


void normalsPixShader(tPixelData& currVrtx,  tMaterialParam* materialParamList, tColorData& out)
{
	out.rgba[0] = (currVrtx.normal[0] + 1.f) * 0.5f;
	out.rgba[1] = (currVrtx.normal[1] + 1.f) * 0.5f;
	out.rgba[2] = (currVrtx.normal[2] + 1.f) * 0.5f;
	out.rgba[3] = 1.f;	
}

void diffuseShader(tPixelData& currVrtx, tMaterialParam* materialParamList, tColorData& out)
{
	tMaterialParam* diffuse = &materialParamList[0];

	out.rgba[0] = diffuse->fValue[0];
	out.rgba[1] = diffuse->fValue[1];
	out.rgba[2] = diffuse->fValue[2];
	out.rgba[3] = 1.f;
}



#endif