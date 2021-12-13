#ifndef PLAYER_H
#define PLAYER_H
#include "world.h"
#include <sstream>

class tPlayer
{
public:
	tPlayer() 
	{
		xM = new tMatrix(4, 1, xvec);
		yM = new tMatrix(4, 1, yvec);
		zM = new tMatrix(4, 1, zvec);
		tmpM = new tMatrix(4, 1, tmpData);
		tmpOut = new tMatrix(4, 1, tmpOutData);
	};
	~tPlayer() {};

	float clamp(float _x, float  _min, float  _max)
	{
		if (_x < _min) return _min;
		if (_x > _max) return _max;
		return _x;
	}
	
	void setLocation(float x, float y, float z) { camera->setLocation(x, y, z); }
	void setAngles(float x, float y, float z) { camera->setAngles(x, y, z); }
	void rotateBy(float x, float y, float z) 
	{ 
		camera->getAngles(fBuf);
		float maxAng = M_PI / 2 - 0.01f;
		float xang = fBuf[0];
		if (xang > M_PI) xang -= M_2PI;
		xang = clamp(xang, - maxAng, maxAng);
		camera->rotateBy(clamp(xang + x, - maxAng, maxAng)-xang, y, z);
	}

	void putXvec(float* trg)
	{
		trg[0] = xvec[0] * speed;
		trg[1] = xvec[1] * speed;
		trg[2] = xvec[2] * speed;
	}
	void putYvec(float* trg)
	{
		trg[0] = yvec[0] * speed;
		trg[1] = yvec[1] * speed;
		trg[2] = yvec[2] * speed;
	}
	void putZvec(float* trg)
	{
		trg[0] = zvec[0] * speed;
		trg[1] = zvec[1] * speed;
		trg[2] = zvec[2] * speed;
	}


	void stepForward(float dt) 
	{ 
		putZvec(tmpData);
		tmpData[2] = -tmpData[2]*dt;
		Multiply(*camera->getRotM2Wmatrix(), *tmpM, tmpOut);
		camera->translate(tmpOut->getValue(0, 0), tmpOut->getValue(1, 0), tmpOut->getValue(2, 0)); 
	}
	void stepBack(float dt) 
	{
		putZvec(tmpData);
		tmpData[2] = tmpData[2] * dt;
		Multiply(*camera->getRotM2Wmatrix(), *tmpM, tmpOut);
		camera->translate(tmpOut->getValue(0, 0), tmpOut->getValue(1, 0), tmpOut->getValue(2, 0));
	}
	void stepLeft(float dt) 
	{ 
		putXvec(tmpData);
		tmpData[0] = -tmpData[0] * dt;
		Multiply(*camera->getRotM2Wmatrix(), *tmpM, tmpOut);
		camera->translate(tmpOut->getValue(0, 0), tmpOut->getValue(1, 0), tmpOut->getValue(2, 0)); 
	}
	void stepRight(float dt) 
	{ 
		putXvec(tmpData);
		tmpData[0] = tmpData[0] * dt;
		Multiply(*camera->getRotM2Wmatrix(), *tmpM, tmpOut);
		camera->translate(tmpOut->getValue(0, 0), tmpOut->getValue(1, 0), tmpOut->getValue(2, 0)); 
	}
	void stepUp(float dt) 
	{
		putYvec(tmpData); 
		tmpData[1] = tmpData[1] * dt;
		Multiply(*camera->getRotM2Wmatrix(), *tmpM, tmpOut);
		camera->translate(tmpOut->getValue(0, 0), tmpOut->getValue(1, 0), tmpOut->getValue(2, 0)); 
	}
	void stepDown(float dt) 
	{
		putYvec(tmpData);
		tmpData[1] = -tmpData[1] * dt;
		Multiply(*camera->getRotM2Wmatrix(), *tmpM, tmpOut);
		camera->translate(tmpOut->getValue(0, 0), tmpOut->getValue(1, 0), tmpOut->getValue(2, 0));
	}


	void commitUpdates()
	{
		camera->UpdateTransforms();
	}

	void setCamera(tCameraObject* _cam) { camera = _cam; }

	void getLocation(float* _x)
	{
		camera->getLocation(_x);
	}

	void getAngles(float* _x)
	{
		camera->getAngles(_x);
	}

private:
	tCameraObject* camera;
	float health;
	float speed = 5.f;
	float unpackParams;

	float xvec[4] = { 1.f, 0.f, 0.f, 1.f };
	float yvec[4] = { 0.f, 1.f, 0.f, 1.f };
	float zvec[4] = { 0.f, 0.f, 1.f, 1.f };
	tMatrix *xM, *yM, *zM;

	float tmpData[4] = { 1.f, 1.f, 1.f, 1.f };
	tMatrix* tmpM;

	float tmpOutData[4] = { 1.f, 1.f, 1.f, 1.f };;
	tMatrix* tmpOut;

	float fBuf[3];




};


#endif
