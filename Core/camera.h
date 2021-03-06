#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include <vector>
#include "transformchain.h"


class tCamera
{
public:
	tCamera() { construct(); };
	tCamera(float _ratio) :ratio(_ratio) 
	{
		construct();
		setParams(ratio);
	};
	~tCamera() 
	{
		delete orthoTransform, perspTransform;
	};
	void construct()
	{
		orthoTransform = new tTransformMatrix(orthoTransformData);
		resetTransformMatrix(orthoTransform);
		perspTransform = new tTransformMatrix(perspTransformData);
		resetTransformMatrix(perspTransform);
		UpdateTransforms();
	}
	void setParams(float _ratio)

	{
		ratio = _ratio;
		height = width / ratio;

		UpdateTransforms();
	}

	void setParams(int _w, int _h)

	{
		ratio = (float)_w/_h;
		height = width / ratio;

		UpdateTransforms();
	}

	void UpdateTransforms()
	{
		orthoTransform->setValue(0, 0, 1.f / width);
		orthoTransform->setValue(1, 1, 1.f / height);
		orthoTransform->setValue(2, 2, -2.f / (zfar - znear));
		orthoTransform->setValue(2, 3, -(zfar + znear) / (zfar - znear));
	}



	tTransformMatrix* getProjectionmatrix()
	{
		return orthoTransform;
	}
	

	
private:
	float ratio;
	float xfov=75, yfov=45;
	float width=5, height=5;
	float znear = 0.f, zfar = 1000.f;

	float orthoTransformData[16], perspTransformData[16];
	tTransformMatrix* orthoTransform;
	tTransformMatrix* perspTransform;
};
#endif
