#include "pch.h"
#include <iostream>
#include "../Core/matrixmath.h"

TEST(MatrixMethods, CheckSetGet) {

	float data[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	tMatrix m(4, 3, data);
	
	EXPECT_EQ(m.getCols(), 3);
	EXPECT_EQ(m.getRows(), 4);
	EXPECT_FLOAT_EQ(m.getValue(2, 2), 8);
	EXPECT_FLOAT_EQ(m.getValue(1, 0), 3);
	EXPECT_FLOAT_EQ(m.getValue(3, 1), 10);
	EXPECT_NO_FATAL_FAILURE(m.setValue(3, 1, 100.f));
	EXPECT_FLOAT_EQ(m.getValue(3, 1), 100);
}


TEST(MatrixOperations, CheckTranspose) {

	float data[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	tMatrix m(4, 3, data);

	float dataOut[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	tMatrix mOut(3, 4, dataOut);

	float refData[12] = { 0, 3, 6, 9, 1, 4, 7, 10, 2, 5, 8, 11 };
	Transpose(m, &mOut);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			EXPECT_FLOAT_EQ(mOut.getValue(i, j), refData[i * 4 + j]);
		}
	}
}


TEST(MatrixOperations, CheckDot) {

	float data1[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	float data2[12] = { 0, 3, 6, 9, 1, 4, 7, 10, 2, 5, 8, 11 };
	float result = dot_row_col(data1, 6, 3, data2, 1, 4);
	EXPECT_FLOAT_EQ(result, 86);
}

TEST(MatrixOperations, CheckMatmul) {

	float data1[6] = { 0, 1, 2, 3, 4, 5};
	tMatrix m1(3, 2, data1);

	float data2[6] = { 6, 7, 8, 9, 10, 11};
	tMatrix m2(2, 3, data2);

	float dataOut[9] = { 0, 1, 2, 3, 4, 5, 6 ,7, 8 };
	tMatrix mOut(3, 3, dataOut);

	Multiply(m1, m2, &mOut);

	float ref[9] = { 9, 10, 11, 39, 44, 49, 69, 78, 87 };
	tMatrix refOut(3, 3, ref);

	for (int i = 0; i < 9; i++)
	{
		EXPECT_FLOAT_EQ(dataOut[i], ref[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			EXPECT_FLOAT_EQ(mOut.getValue(i, j), refOut.getValue(i, j));
		}
	}
}
