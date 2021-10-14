#ifndef TRANSFORMCHAIN_H
#define TRANSFORMCHAIN_H

#include "matrixmath.h"
#include "mesh.h"
#include "mathutils.h"

float M_PI = 3.14159274101257324219;
float M_2PI = 2 * M_PI;

float defaultTransformData[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
tMatrix defaultTransform(4, 4, defaultTransformData);


float maxAngle = 2 * M_PI;
float minAngle = -2 * M_PI;


void resetTransformMatrix(float* m)
{
	memcpy(m, defaultTransformData, 16 * sizeof(float));	
}

void resetTransformMatrix(tMatrix* m)
{
	resetTransformMatrix(m->getDataPtr());
}

void coords2TranslationMatrix(float x, float y, float z, tMatrix* m)
{
	m->setValue(0, 3, x);
	m->setValue(1, 3, y);
	m->setValue(2, 3, z);
}

void scales2ScaleMatrix(float x, float y, float z, tMatrix* m)
{
	m->setValue(0, 0, x);
	m->setValue(1, 1, y);
	m->setValue(2, 2, z);
}

void rot2Matr(float yaw, float pitch, float roll, tMatrix* mY, tMatrix* mZ, tMatrix* mX)
{
	float cos_val = cos(yaw);
	float sin_val = sin(yaw);

	mY->setValue(0, 0, cos_val);
	mY->setValue(2, 2, cos_val);
	mY->setValue(0, 2, sin_val);
	mY->setValue(2, 0, -sin_val);


	cos_val = cos(pitch);
	sin_val = sin(pitch);

	mZ->setValue(0, 0, cos_val);
	mZ->setValue(1, 1, cos_val);
	mZ->setValue(0, 1, -sin_val);
	mZ->setValue(1, 0, sin_val);

	cos_val = cos(roll);
	sin_val = sin(roll);

	mX->setValue(1, 1, cos_val);
	mX->setValue(2, 2, cos_val);
	mX->setValue(1, 2, -sin_val);
	mX->setValue(2, 1, sin_val);
}




class tTransformChain
{
public:
	tTransformChain()
	{ 
		initObject(); 
	};
	~tTransformChain()
	{
		delete ScM2W, RotXM2W, RotYM2W, RotZM2W, TranslM2W, TransfM2W, TransfW2M;
	};

	void initObject()
	{
		ScM2W = new tMatrix(4, 4, ScM2Wdata);
		resetTransformMatrix(ScM2W);
		ScW2M = new tMatrix(4, 4, ScW2Mdata);
		resetTransformMatrix(ScW2M);
		setScale(scX, scY, scZ);
		
		RotXM2W = new tMatrix(4, 4, RotXM2Wdata);
		RotYM2W = new tMatrix(4, 4, RotYM2Wdata);
		RotZM2W = new tMatrix(4, 4, RotZM2Wdata);
		resetTransformMatrix(RotXM2W);
		resetTransformMatrix(RotYM2W);
		resetTransformMatrix(RotZM2W);
		setAngles(yaw, pitch, roll);
		
		TranslM2W = new tMatrix(4, 4, TranslM2Wdata);
		resetTransformMatrix(TranslM2W);
		TranslW2M = new tMatrix(4, 4, TranslW2Mdata);
		resetTransformMatrix(TranslW2M);
		setLocation(x, y, z);

		TransfM2W = new tMatrix(4, 4, TransfM2Wdata);
		TransfW2M = new tMatrix(4, 4, TransfW2Mdata);
		tmpTransform = new tMatrix(4, 4, tmpTransformData);

		UpdateTransforms();
	}

	

	void setLocation(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
		coords2TranslationMatrix(x, y, z, TranslM2W);
		coords2TranslationMatrix(-x, -y, -z, TranslW2M);
	}

	void translate(float dx, float dy, float dz)
	{	
		x += dx;
		y += dy;
		z += dz;
		setLocation(x, y, z);
	}

	void setScale(float _x, float _y, float _z)
	{
		scX = _x;
		scY = _y;
		scZ = _z;
		
		scales2ScaleMatrix(scX, scY, scZ, ScM2W);
		scales2ScaleMatrix(1.f / scX, 1.f / scY, 1.f / scZ, ScW2M);
	}

	void ScaleBy(float ScaleX, float ScaleY, float ScaleZ)
	{
		scX *= ScaleX;
		scY *= ScaleY;
		scZ *= ScaleZ;
		setScale(scX, scY, scZ);
	}

	void setAngles(float rotX, float rotY, float rotZ )
	{
		yaw = removePeriodic(rotY, 0, M_2PI);
		pitch = removePeriodic(rotZ, 0, M_2PI);
		roll = removePeriodic(rotX, 0, M_2PI);
		rot2Matr(yaw, pitch, roll, RotYM2W, RotZM2W, RotXM2W);
	}

	void rotateBy(float rotX, float rotY, float rotZ)
	{
		yaw += rotY;
		pitch += rotZ;
		roll += rotX;
		setAngles(yaw, pitch, roll);
	}

	
	void UpdateTransforms()
	{

		//Multiply(*TranslM2W, *RotXM2W, tmpTransform);
		//Multiply(*tmpTransform, *RotZM2W, TransfM2W);
		//Multiply(*TransfM2W, *RotYM2W, tmpTransform);
		//Multiply(*tmpTransform, *ScM2W, TransfM2W);

		//Multiply(*ScW2M, *RotXW2M, tmpTransform);
		//Multiply(*tmpTransform, *RotZW2M, TransfW2M);
		//Multiply(*TransfW2M, *RotYW2M, tmpTransform);
		//Multiply(*tmpTransform, *TranslW2M, TransfW2M);


		Multiply(*RotXM2W, *RotZM2W, tmpTransform);
		Multiply(*tmpTransform, *RotYM2W, TransfM2W);
		Transpose(*TransfM2W, TransfW2M);
		
		Multiply(*TranslM2W, *TransfM2W, tmpTransform);
		Multiply(*tmpTransform, *ScM2W, TransfM2W);

		
		Multiply(*ScW2M, *TransfW2M, tmpTransform);
		Multiply(*tmpTransform, *TranslW2M, TransfW2M);

		
	}

	tMatrix* getM2Wmatrix()
	{
		return TransfM2W;
	}

	tMatrix* getW2Mmatrix()
	{
		return TransfW2M;
	}

	static void makeTransform(float x, float y, float z, tMatrix* transformMatrix, float* result)
	{
		float inp[4] = { 1., 1., 1., 1. };
		tMatrix coordsIn(4, 1, inp);
		float arr[3] = { x, y, z };
		arr2Matrix(arr, coordsIn, 3);

		float res[4];
		tMatrix coordsOut(4, 1, res);

		Multiply(*transformMatrix, coordsIn, &coordsOut);
		matrix2Arr(coordsOut, result, 3);
	}

	void Model2World(float x, float y, float z, float* result)
	{
		makeTransform(x, y, z, TransfM2W, result);
	}

	void World2Model(float x, float y, float z, float* result)
	{
		makeTransform(x, y, z, TransfW2M, result);
	}



private:
	float ScM2Wdata[16], RotXM2Wdata[16], RotYM2Wdata[16], RotZM2Wdata[16], TranslM2Wdata[16], TransfM2Wdata[16], tmpTransformData[16];
	tMatrix *ScM2W, *RotXM2W, *RotYM2W, *RotZM2W, *TranslM2W, *TransfM2W, *tmpTransform;

	float ScW2Mdata[16], TranslW2Mdata[16], TransfW2Mdata[16];
	tMatrix *ScW2M, *TranslW2M, *TransfW2M;


	float scX = 1, scY = 1, scZ = 1;
	float x = 0, y = 0, z = 0;
	float yaw = 0, pitch = 0, roll = 0;
};


#endif