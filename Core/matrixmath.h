#ifndef MATRIXMATH_H
#define MATRIXMATH_H

#include <cstring>
class tMatrix
{
public:
	tMatrix() {};
	tMatrix(int _rows, int _cols, float* _data) : rows(_rows), cols(_cols), data(_data) {};
	~tMatrix() {};

	int getRows() { return rows; }
	int getCols() { return cols; }
	float getValue(int i, int j) { return data[i * cols + j]; }
	void setValue(int i, int j, float x) { data[i * cols + j] = x; }
	float* getDataPtr() { return data; }

protected:
	int rows;
	int cols;
private:	
	float* data;
};

inline float dot_row_col(float* data1, int start1, int len, float* data2, int start2, int stride)
{
	float res = 0.0f;
	for (int i = 0; i < len; i++)
	{
		res += data1[start1 + i] * data2[start2 + i * stride];
	}
	return res;
}


inline void Multiply(tMatrix& m1, tMatrix& m2, tMatrix* result)
{
	if (m1.getCols() != m2.getRows()) { return; }

	for (int i = 0; i < m1.getRows(); i++)
	{
		for (int j = 0; j < m2.getCols(); j++)
		{
			result->setValue(i, j, dot_row_col(m1.getDataPtr(), m1.getCols() * i, m1.getCols(),  m2.getDataPtr(), j, m2.getCols()));
		}
	}
}

inline void Multiply(tMatrix& m1, float number, tMatrix* result)
{
	for (int i = 0; i < m1.getRows(); i++)
	{
		for (int j = 0; j < m1.getCols(); j++)
		{
			result->setValue(i, j, m1.getValue(i, j) * number);
		}
	}
}


inline void Transpose(tMatrix& m1, tMatrix* result)
{
	for (int i = 0; i < m1.getRows(); i++)
	{
		for (int j = 0; j < m1.getCols(); j++)
		{
			result->setValue(j, i, m1.getValue(i, j));
		}
	}
}

inline void arr2Matrix(float* arr, tMatrix& cMatr, int size = -1)
{
	if (size == -1)  size = cMatr.getRows()*cMatr.getCols();
	memcpy(cMatr.getDataPtr(), arr, size * sizeof(float));	
}

inline void matrix2Arr(tMatrix& cMatr, float* coordsOut, int size = -1)
{
	if (size == -1)  size = cMatr.getRows()*cMatr.getCols();
	memcpy(coordsOut, cMatr.getDataPtr(), size * sizeof(float));
}

#endif