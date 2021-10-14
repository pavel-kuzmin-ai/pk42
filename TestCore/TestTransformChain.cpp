#include "pch.h"
#include <iostream>
#include "../Core/transformchain.h"

float tol = 1.e-5f;
TEST(Transforms, CheckTranslations) 
{
	tTransformChain mvbl;
	float result[3];
	mvbl.Model2World(10.f, 20.f, 30.f, result);
	
	EXPECT_FLOAT_EQ(result[0], 10.f);
	EXPECT_FLOAT_EQ(result[1], 20.f);
	EXPECT_FLOAT_EQ(result[2], 30.f);

	mvbl.setLocation(10.f, 10.f, 10.f);
	mvbl.UpdateTransforms();
	mvbl.Model2World(10.f, 20.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 20.f);
	EXPECT_FLOAT_EQ(result[1], 30.f);
	EXPECT_FLOAT_EQ(result[2], 40.f);

	mvbl.translate(-7.f, 32.f, 81.f);
	mvbl.UpdateTransforms();
	mvbl.Model2World(10.f, 20.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 13.f);
	EXPECT_FLOAT_EQ(result[1], 62.f);
	EXPECT_FLOAT_EQ(result[2], 121.f);
}
TEST(Transforms, CheckScale) 
{

	tTransformChain mvbl;
	float result[3];
	
	mvbl.setScale(2.f, 3.f, 0.1f);
	mvbl.UpdateTransforms();
	mvbl.Model2World(10.f, 20.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 20.f);
	EXPECT_FLOAT_EQ(result[1], 60.f);
	EXPECT_FLOAT_EQ(result[2], 3.f);
}
TEST(Transforms, CheckBasisRotations) 
{

	tTransformChain mvbl;
	float result[3];

	mvbl.setLocation(0.f, 0.f, 0.f);
	mvbl.setAngles(0, M_PI / 3.f, 0);
	mvbl.UpdateTransforms();
	
	mvbl.Model2World(10, 0, 0, result);
	EXPECT_FLOAT_EQ(result[0], 5.f);
	EXPECT_FLOAT_EQ(result[1], 0.f);
	EXPECT_FLOAT_EQ(result[2], -10.f * sqrt(3.f) / 2.f);

	mvbl.Model2World(0.f, 0.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 30.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[1], 0.f);
	EXPECT_FLOAT_EQ(result[2], 15.f);

	mvbl.setAngles(0.f, 0.f, M_PI / 3.f);
	mvbl.UpdateTransforms();

	mvbl.Model2World(10.f, 0.f, 0.f, result);
	EXPECT_FLOAT_EQ(result[0], 5.f);
	EXPECT_FLOAT_EQ(result[1], 10.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[2], 0.f);

	mvbl.Model2World(0.f, 20.f, 0.f, result);
	EXPECT_FLOAT_EQ(result[0], - 20.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[1], 10.f);
	EXPECT_FLOAT_EQ(result[2], 0.f);

	mvbl.setAngles(M_PI / 3.f, 0.f, 0.f);
	mvbl.UpdateTransforms();

	mvbl.Model2World(0.f, 20.f, 0.f, result);
	EXPECT_FLOAT_EQ(result[0], 0.f);
	EXPECT_FLOAT_EQ(result[1], 10.f);
	EXPECT_FLOAT_EQ(result[2], 20.f * sqrt(3.f) / 2.f);

	mvbl.Model2World(0.f, 0.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 0.f);
	EXPECT_FLOAT_EQ(result[1], -30.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[2], 15.f);
}
TEST(Transforms, Check2DVecRotations) 
{

	tTransformChain mvbl;
	float result[3];

	mvbl.setLocation(0.f, 0.f, 0.f);
	mvbl.setAngles(0.f, M_PI / 3.f, 0.f);
	mvbl.UpdateTransforms();
	
	mvbl.Model2World(10.f, 0.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 5.f + 30.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[1], 0.f);
	EXPECT_FLOAT_EQ(result[2], -10.f * sqrt(3.f) / 2.f + 15.f);

	mvbl.setAngles(0.f, 0.f, M_PI / 3.f);
	mvbl.UpdateTransforms();

	mvbl.Model2World(10.f, 20.f, 0.f, result);
	EXPECT_FLOAT_EQ(result[0], 5.f - 20.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[1], 10.f * sqrt(3.f) / 2.f + 10.f);
	EXPECT_FLOAT_EQ(result[2], 0.f);

	

	mvbl.setAngles(M_PI / 3.f, 0.f, 0.f);
	mvbl.UpdateTransforms();

	mvbl.Model2World(0.f, 20.f, 30.f, result);
	EXPECT_FLOAT_EQ(result[0], 0.f);
	EXPECT_FLOAT_EQ(result[1], 10.f - 30.f * sqrt(3.f) / 2.f);
	EXPECT_FLOAT_EQ(result[2], 20.f * sqrt(3.f) / 2.f + 15.f);
}
TEST(Transforms, CheckTransformationChain) 
{

	tTransformChain mvbl;
	float result[3];

	mvbl.setScale(2.f, 1.f, 3.f);
	mvbl.setLocation(7.f, 11.f, 19.f);
	mvbl.setAngles(0.f, M_PI / 3.f, 0.f);
	mvbl.UpdateTransforms();

	mvbl.Model2World(5.f, 0.f, 10.f, result);
	EXPECT_FLOAT_EQ(result[0], 5.f + 30.f * sqrt(3.f) / 2.f + 7.f);
	EXPECT_FLOAT_EQ(result[1], 0.f + 11.f);
	EXPECT_FLOAT_EQ(result[2], -10.f * sqrt(3.f) / 2.f + 15.f + 19.f);
}
TEST(Transforms, CheckInverseTransformationChain) {

	tTransformChain mvbl;
	float result[3];

	mvbl.setScale(1.f, 1.f, 1.f);
	mvbl.setLocation(18.f, 0.f, 10.f);
	mvbl.setAngles(0.f, -M_PI / 6.f, 0.f);
	mvbl.UpdateTransforms();

	mvbl.World2Model(20.f, 0.f, 10.f, result);
	EXPECT_NEAR(result[0], 2.f * sqrt(3.f) / 2.f, tol);
	EXPECT_NEAR(result[1], 0.f, tol);
	EXPECT_NEAR(result[2], -1.f, tol);

	mvbl.setScale(1.f, 1.f, 1.f);
	mvbl.setLocation(18.f, 0.f, 10.f);
	mvbl.setAngles(0.f, -M_PI / 4.f, 0.f);
	mvbl.UpdateTransforms();

	mvbl.World2Model(20.f, 0.f, 10.f, result);
	EXPECT_NEAR(result[0], 2.f * sqrt(2.f) / 2.f, tol);
	EXPECT_NEAR(result[1], 0.f, tol);
	EXPECT_NEAR(result[2], -2.f * sqrt(2.f) / 2.f, tol);
}

