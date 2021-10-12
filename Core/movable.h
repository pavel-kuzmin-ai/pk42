#ifndef MOVABLE_H
#define MOVABLE_H

#include "matrixmath.h"
#include "mesh.h"

#define M_PI 3.14159274101257324219

float defaultTransformData[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
tMatrix defaultTransform(4, 4, defaultTransformData);


float maxAngle = 2 * M_PI;
float minAngle = -2 * M_PI;


void resetTransformMatrix(float* m)
{
	for (int i = 0; i < 16; i++)
	{
		m[i] = defaultTransformData[i];
	}
}

void resetTransformMatrix(tMatrix* m)
{
	for (int i = 0; i < m->getRows(); i++)
	{
		for (int j = 0; j < m->getCols(); j++)
		{
			m->setValue(i, j, defaultTransform.getValue(i, j));
		}
	}
}

void coords2TranslationMatrix(float x, float y, float z, tMatrix* m)
{
	m->setValue(0, 3, x);
	m->setValue(1, 3, y);
	m->setValue(2, 3, z);
}

void scales2ScaleMatrix(float x, float y, float z, tMatrix* m)
{
	m->setValue(0, 3, x);
	m->setValue(1, 3, y);
	m->setValue(2, 3, z);
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
	mZ->setValue(0, 1, sin_val);
	mZ->setValue(1, 0, -sin_val);

	cos_val = cos(roll);
	sin_val = sin(roll);

	mX->setValue(1, 1, cos_val);
	mX->setValue(2, 2, cos_val);
	mX->setValue(1, 2, sin_val);
	mX->setValue(2, 1, -sin_val);
}

float removePeriodic(float inp)
{

	while (inp > maxAngle)
	{
		inp -= maxAngle;
	}
	while (inp < minAngle)
	{
		inp -= minAngle;
	}

	return inp;
}


class tMovable
{
public:
	tMovable() 
	{ 
		initObject(); 
	};
	~tMovable() 
	{
		delete ScM2W, RotXM2W, RotYM2W, RotZM2W, TranslM2W, TransfM2W, tmpMatrix;
	};

	void initObject()
	{
		ScM2W = new tMatrix(4, 4, ScM2Wdata);
		resetTransformMatrix(ScM2W);
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
		setLocation(x, y, z);

		TransfM2W = new tMatrix(4, 4, TransfM2Wdata);
		tmpMatrix = new tMatrix(4, 4, tmpMatrixData);

		UpdateModel2WorldMatrix();
	}

	

	void setLocation(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
		coords2TranslationMatrix(x, y, z, TranslM2W);
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
		scales2ScaleMatrix(x, y, z, ScM2W);
	}

	void ScaleBy(float ScaleX, float ScaleY, float ScaleZ)
	{
		scX *= ScaleX;
		scY *= ScaleY;
		scZ *= ScaleZ;
		setScale(scX, scY, scZ);
	}

	void setAngles(float rotY, float rotZ, float rotX)
	{
		yaw = removePeriodic(rotY);
		pitch = removePeriodic(rotZ);
		roll = removePeriodic(rotX);
		rot2Matr(yaw, pitch, roll, RotXM2W, RotYM2W, RotZM2W);
	}

	void rotateBy(float rotY, float rotZ, float rotX)
	{
		yaw += rotY;
		pitch += rotZ;
		roll += rotX;
		setAngles(yaw, pitch, roll);
	}

	
	void UpdateModel2WorldMatrix()
	{
		Multiply(*TranslM2W, *RotZM2W, tmpMatrix);
		Multiply(*tmpMatrix, *RotYM2W, TransfM2W);
		Multiply(*TransfM2W, *RotXM2W, tmpMatrix);
		Multiply(*tmpMatrix, *ScM2W, TransfM2W);
	}

	tMatrix* getTransformMatrix()
	{
		return TransfM2W;
	}

private:
	float ScM2Wdata[16], RotXM2Wdata[16], RotYM2Wdata[16], RotZM2Wdata[16], TranslM2Wdata[16], TransfM2Wdata[16], tmpMatrixData[16];
	tMatrix *ScM2W, *RotXM2W, *RotYM2W, *RotZM2W, *TranslM2W, *TransfM2W, *tmpMatrix;

	float scX = 1, scY = 1, scZ = 1;
	float x = 0, y = 0, z = 0;
	float yaw = 0, pitch = 0, roll = 0;
};


void Model2World(float x, float y, float z, tMovable* movable, float* result)
{
	float inp[4];
	inp[0] = x;
	inp[1] = y;
	inp[2] = z;
	inp[3] = 1.f;
	tMatrix coordsIn(4, 1, inp);

	float res[4];
	tMatrix coordsOut(4, 1, res);

	Multiply(*movable->getTransformMatrix(), coordsIn, &coordsOut);

	result[0] = coordsOut.getValue(0, 0);
	result[1] = coordsOut.getValue(1, 0);
	result[2] = coordsOut.getValue(2, 0);
}

#endif