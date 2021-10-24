#ifndef PLAYER_H
#define PLAYER_H
#include "world.h"
#include <sstream>

class tPlayer
{
public:
	tPlayer() {};
	~tPlayer() {};
	
	void setLocation(float x, float y, float z) { camera->setLocation(x, y, z); }
	void setAngles(float x, float y, float z) { camera->setAngles(x, y, z); }

	void stepForward(float dt) { camera->translate(0.f, 0.f, -speed * dt); }
	void stepBack(float dt) { camera->translate(0.f, 0.f, speed * dt); }
	void stepLeft(float dt) { camera->translate(-speed * dt, 0.f, 0.f); }
	void stepRight(float dt) { camera->translate(speed * dt, 0.f, 0.f); }
	void stepUp(float dt) { camera->translate( 0.f, speed * dt, 0.f); }
	void stepDown(float dt) { camera->translate(0.f, -speed * dt, 0.f); }


	void commitUpdates()
	{
		camera->UpdateTransforms();
	}

	void setCamera(tCameraObject* _cam) { camera = _cam; }


private:
	tCameraObject* camera;
	float health;
	float speed;
	float unpackParams;



};


#endif
