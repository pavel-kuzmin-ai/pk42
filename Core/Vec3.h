#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include "mathutils.h"

template <typename T>
class vec3
{
public:
	vec3() : e{ 0,0,0 } {};
	vec3(T x, T y, T z) : e{ x,y,z } {};
	~vec3() {};

	T x() const { return e[0]; }
	T y() const { return e[1]; }
	T z() const { return e[2]; }

	vec3<T> operator-() const { return vec3<T>(-e[0], -e[1], -e[2]); }
	T operator[](int i) const { return e[i]; }
	T& operator[](int i) { return e[i]; }

	vec3<T>& operator+=(const vec3<T> &v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3<T>& operator*=(const T t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3<T>& operator*=(const vec3<T> &v) {
		e[0] *= v.e[0];
		e[1] *= v.e[1];
		e[2] *= v.e[2];
		return *this;
	}

	vec3<T>& operator/=(const T t) {
		return *this *= 1 / t;
	}

	T length() const {
		return sqrt(length_squared());
	}

	T length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

public:
	T e[3];
};

// Type aliases for vec3
using fvec3 = vec3<float>;
using point3 = fvec3;   // 3D point
using color = fvec3;
using colorRGB = vec3<int>;    // RGB color



// vec3 Utility Functions

template <typename T>
inline std::ostream& operator<<(std::ostream &out, const vec3<T> &v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

template <typename T>
inline vec3<T> operator+(const vec3<T> &u, const vec3<T> &v) {
	return vec3<T>(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

template <typename T>
inline vec3<T> operator-(const vec3<T> &u, const vec3<T> &v) {
	return vec3<T>(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

template <typename T>
inline vec3<T> operator*(const vec3<T> &u, const vec3<T> &v) {
	return vec3<T>(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

template <typename T>
inline vec3<T> operator*(T t, const vec3<T> &v) {
	return vec3<T>(t*v.e[0], t*v.e[1], t*v.e[2]);
}

template <typename T>
inline vec3<T> operator*(float t, const vec3<T> &v) {
	T t_conv = (T)t;
	return vec3<T>((T)(t*(float)v.e[0]), 
		           (T)(t*(float)v.e[1]), 
		           (T)(t*(float)v.e[2]));
}

template <typename T>
inline vec3<T> operator*(const vec3<T> &v, T t) {
	return t * v;
}

template <typename T>
inline vec3<T> operator/(vec3<T> v, T t) {
	return (1 / t) * v;
}

template <typename T>
inline T dot(const vec3<T> &u, const vec3<T> &v) {
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}

template <typename T>
inline vec3<T> cross(const vec3<T> &u, const vec3<T> &v) {
	return vec3<T>(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

template <typename T>
inline vec3<T> unit_vector(vec3<T> v) {
	return v / v.length();
}


void rgb2hsv(const vec3<float> &inRGB, vec3<float> &outHSV)
{
	int _maxIdx = argMax(inRGB.e, 3);
	int _minIdx = argMin(inRGB.e, 3);

	float _max = inRGB[_maxIdx];
	float _min = inRGB[_minIdx];

	float h = 0;
	float s = _max == 0 ? 0 : 1.f - _min / _max;
	float v = _max;

	outHSV[0] = h;
	outHSV[1] = s;
	outHSV[2] = v;

	float mult = 60.f / (_max - _min);

	if (_min == _max) return;

	if (_maxIdx == 0)
	{
		h = (inRGB[1] - inRGB[2])*mult;
		outHSV[0] = inRGB[1] >= inRGB[2] ? h : h + 360.f;
		return;
	}

	if (_maxIdx == 1)
	{
		outHSV[0] = (inRGB[2] - inRGB[0])*mult + 120.f;
		return;
	}

	if (_maxIdx == 0)
	{
		outHSV[0] = (inRGB[0] - inRGB[1])*mult + 240.f;
	}
}
#endif