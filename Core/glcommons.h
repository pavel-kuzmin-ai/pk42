#ifndef GLCOMMONS_H
#define GLCOMMONS_H

#include <unordered_map>

struct tInOutVertex
{
	float x[3];
};

struct tIntInOutVertex
{
	int x[3];
};

struct tPixelData
{
	float xyz[3];
	float uv[2];
	float normal[3];
	float xyzCam[3];
	float viewDirection[3];
};

struct tTriangleData
{
	tPixelData* pixels[3];
	float normal[3];
	float viewDirection[3];
};

struct tColorData
{
	float rgba[4];
};

struct fBuffer
{
	float* fBuf;
	int width, height, channels;
};


typedef std::unordered_map<std::string, fBuffer*> tNamedBuffers;

void cross(const float* u, const float* v, float* out)
{
	out[0] = u[1] * v[2] - u[2] * v[1];
	out[1] = u[2] * v[0] - u[0] * v[2];
	out[2] = u[0] * v[1] - u[1] * v[0];
}

float dot(const float* u, const float* v, int size=3)
{
	float out = 0;
	for (int i = 0; i < size; i++)
	{
		out += u[i] * v[i];
	}
	return out;	
}



inline float getLsq(float* x)
{
	return x[0] * x[0] + x[1] * x[1] + x[2] * x[2];
}


inline float euclideanDist(float* x)
{
	return std::sqrt(getLsq(x));
}


void normalize(float* x)
{
	float mult = 1.f / euclideanDist(x);
	x[0] *= mult;
	x[1] *= mult;
	x[2] *= mult;
}

void setFaceNormals(tPixelData& p0, tPixelData& p1, tPixelData& p2)
{
	float v1[3], v2[3];

	v1[0] = p1.xyz[0] - p0.xyz[0];
	v1[1] = p1.xyz[1] - p0.xyz[1];
	v1[2] = p1.xyz[2] - p0.xyz[2];

	v2[0] = p2.xyz[0] - p0.xyz[0];
	v2[1] = p2.xyz[1] - p0.xyz[1];
	v2[2] = p2.xyz[2] - p0.xyz[2];
	cross(v1, v2, p0.normal);
	normalize(p0.normal);
	std::memcpy(p1.normal, p0.normal, 3 * sizeof(float));
	std::memcpy(p2.normal, p0.normal, 3 * sizeof(float));
};

void setFaceNormal(tTriangleData& tri)
{
	float v1[3], v2[3];

	v1[0] = tri.pixels[1]->xyz[0] - tri.pixels[0]->xyz[0];
	v1[1] = tri.pixels[1]->xyz[1] - tri.pixels[0]->xyz[1];
	v1[2] = tri.pixels[1]->xyz[2] - tri.pixels[0]->xyz[2];

	v2[0] = tri.pixels[2]->xyz[0] - tri.pixels[0]->xyz[0];
	v2[1] = tri.pixels[2]->xyz[1] - tri.pixels[0]->xyz[1];
	v2[2] = tri.pixels[2]->xyz[2] - tri.pixels[0]->xyz[2];
	cross(v1, v2, tri.normal);
	normalize(tri.normal);
};

void calcFaceNormals(tPixelData& p0, tPixelData& p1, tPixelData& p2, float* out)
{
	float v1[3], v2[3];

	v1[0] = p1.xyz[0] - p0.xyz[0];
	v1[1] = p1.xyz[1] - p0.xyz[1];
	v1[2] = p1.xyz[2] - p0.xyz[2];

	v2[0] = p2.xyz[0] - p0.xyz[0];
	v2[1] = p2.xyz[1] - p0.xyz[1];
	v2[2] = p2.xyz[2] - p0.xyz[2];
	cross(v1, v2, out);
	normalize(out);
};

void calcFaceNormalsCam(tPixelData& p0, tPixelData& p1, tPixelData& p2, float* out)
{
	float v1[3], v2[3];

	v1[0] = p1.xyzCam[0] - p0.xyzCam[0];
	v1[1] = p1.xyzCam[1] - p0.xyzCam[1];
	v1[2] = p1.xyzCam[2] - p0.xyzCam[2];

	v2[0] = p2.xyzCam[0] - p0.xyzCam[0];
	v2[1] = p2.xyzCam[1] - p0.xyzCam[1];
	v2[2] = p2.xyzCam[2] - p0.xyzCam[2];
	cross(v1, v2, out);
	normalize(out);
};


struct tBbox
{
	float xmin, xmax, ymin, ymax, zmin, zmax;
};

struct tBboxInt
{
	int xmin, xmax, ymin, ymax;
	float zmin, zmax;
};


float edgePointDoubleArea(tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& testVrtx)
{
	return (vrtx1.xyz[0] - vrtx0.xyz[0]) * (testVrtx.xyz[1] - vrtx0.xyz[1]) - (vrtx1.xyz[1] - vrtx0.xyz[1]) * (testVrtx.xyz[0] - vrtx0.xyz[0]);
}

void setBbox(tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2, tBbox& box, float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	
	box.xmin = std::fmaxf(std::fminf(vrtx0.xyz[0], std::fminf(vrtx1.xyz[0], vrtx2.xyz[0])), minX);
	box.xmax = std::fminf(std::fmaxf(vrtx0.xyz[0], std::fmaxf(vrtx1.xyz[0], vrtx2.xyz[0])), maxX);

	box.ymin = std::fmaxf(std::fminf(vrtx0.xyz[1], std::fminf(vrtx1.xyz[1], vrtx2.xyz[1])), minY);
	box.ymax = std::fminf(std::fmaxf(vrtx0.xyz[1], std::fmaxf(vrtx1.xyz[1], vrtx2.xyz[1])), maxY);

	box.zmin = std::fmaxf(std::fminf(vrtx0.xyz[2], std::fminf(vrtx1.xyz[2], vrtx2.xyz[2])), minZ);
	box.zmax = std::fminf(std::fmaxf(vrtx0.xyz[2], std::fmaxf(vrtx1.xyz[2], vrtx2.xyz[2])), maxZ);
	
}

inline int imax(int x, int y)
{
	if (x >= y) return x;
	return y;
}

inline int imin(int x, int y)
{
	if (x <= y) return x;
	return y;
}

void setBbox(tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2, tBboxInt& box, int minX, int minY, float minZ, int maxX, int maxY, float maxZ)
{

	box.xmin = imax(int(std::fminf(vrtx0.xyz[0], std::fminf(vrtx1.xyz[0], vrtx2.xyz[0])) + 0.5f), minX);
	box.xmax = imin(int(std::fmaxf(vrtx0.xyz[0], std::fmaxf(vrtx1.xyz[0], vrtx2.xyz[0])) + 0.5f), maxX);

	box.ymin = imax(int(std::fminf(vrtx0.xyz[1], std::fminf(vrtx1.xyz[1], vrtx2.xyz[1]))), minY);
	box.ymax = imin(int(std::fmaxf(vrtx0.xyz[1], std::fmaxf(vrtx1.xyz[1], vrtx2.xyz[1]))), maxY);

	box.zmin = std::fmaxf(std::fminf(vrtx0.xyz[2], std::fminf(vrtx1.xyz[2], vrtx2.xyz[2])), minZ);
	box.zmax = std::fminf(std::fmaxf(vrtx0.xyz[2], std::fmaxf(vrtx1.xyz[2], vrtx2.xyz[2])), maxZ);

}

void upscaleCoords(tPixelData& _vrtx, int width, int height)
{
	_vrtx.xyz[0] = (_vrtx.xyz[0] + 1.f)*0.5f*width;
	_vrtx.xyz[1] = (_vrtx.xyz[1] + 1.f)*0.5f*height;
}

void setBuf(int* buf, int val, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	buf[idx + channelIdx] = val;
}

void setBuf(float* buf, float val, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	buf[idx + channelIdx] = val;
}

int getBuf(int* buf, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	return buf[idx + channelIdx];
}

inline float getBuf(float* buf, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	return buf[idx + channelIdx];
}

inline float weightedTriSum(float x, float y, float z, float wx, float wy, float wz)
{
	return x * wx + y * wy + z * wz;
}


void interpolatePixelData(tPixelData& res, tPixelData& pxl0, tPixelData& pxl1, tPixelData& pxl2, float &lambda0, float &lambda1, float &lambda2)
{
	

	for (int i = 0; i < 2; i++)
	{
		res.uv[i] = weightedTriSum(pxl0.uv[i], pxl1.uv[i], pxl2.uv[i], lambda0, lambda1, lambda2);
	}

	
	
	for (int i = 0; i < 3; i++)
	{
		res.normal[i] = weightedTriSum(pxl0.normal[i], pxl1.normal[i], pxl2.normal[i], lambda0, lambda1, lambda2);
	}
	

	for (int i = 0; i < 3; i++)
	{
		res.viewDirection[i] = weightedTriSum(pxl0.viewDirection[i], pxl1.viewDirection[i], pxl2.viewDirection[i], lambda0, lambda1, lambda2);
	}
}

bool getLineParams(float x0s, float y0s, float x0e, float y0e, float *params)
{
	float k = 0;
	float b = 0;
	bool isVert = false;

	if (x0s == x0e) isVert = true;
	else
	{
		k = (y0e - y0s) / (x0e - x0s);
		b = y0s - k * x0s;
	}
	params[0] = k;
	params[1] = b;
	return isVert;
}

inline void swap(float* x, float* y)
{
	float tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;
}
bool intersectionParams(float* r0, float* r1, float *out)
{
	float a00 = r0[2] - r0[0];
	float a01 = -(r1[2] - r1[0]);
	float a10 = r0[3] - r0[1];
	float a11 = -(r1[3] - r1[1]);

	float b0 = r1[0] - r0[0];
	float b1 = r1[1] - r0[1];

	
	float det = a00 * a11 - a01 * a10;
	if (det == 0) return false;
	
	det = 1.f / det;
	float t = (b0 * a11 - a01 * b1)*det;
	float s = (a00 * b1 - b0 * a10)*det;

	out[0] = t;
	out[1] = s;
	return true;
}


inline float clamp(float x, float minimum, float maximum)
{
	float out = x;
	if (out < minimum) out = minimum;
	if (out > maximum) out = maximum;
	return out;
}

inline int clamp(int x, int minimum, int maximum)
{
	int out = x;
	if (out < minimum) out = minimum;
	if (out > maximum) out = maximum;
	return out;
}

#endif // !GLCOMMONS_H

