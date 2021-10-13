#include "pch.h"
#include <iostream>
#include "../Core/transformchain.h"

TEST(Transforms, CheckTranslations) 
{
	tTransformChain mvbl;
	float result[3];
	Model2World(10, 20, 30, &mvbl, result);
	
	EXPECT_FLOAT_EQ(result[0], 10);
	EXPECT_FLOAT_EQ(result[1], 20);
	EXPECT_FLOAT_EQ(result[2], 30);

	mvbl.setLocation(10, 10, 10);
	mvbl.UpdateTransforms();
	Model2World(10, 20, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 20);
	EXPECT_FLOAT_EQ(result[1], 30);
	EXPECT_FLOAT_EQ(result[2], 40);

	mvbl.translate(-7, 32, 81);
	mvbl.UpdateTransforms();
	Model2World(10, 20, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 13);
	EXPECT_FLOAT_EQ(result[1], 62);
	EXPECT_FLOAT_EQ(result[2], 121);
}
TEST(Transforms, CheckScale) 
{

	tTransformChain mvbl;
	float result[3];
	
	mvbl.setScale(2, 3, 0.1);
	mvbl.UpdateTransforms();
	Model2World(10, 20, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 20);
	EXPECT_FLOAT_EQ(result[1], 60);
	EXPECT_FLOAT_EQ(result[2], 3);
}
TEST(Transforms, CheckBasisRotations) 
{

	tTransformChain mvbl;
	float result[3];

	mvbl.setLocation(0, 0, 0);
	mvbl.setAngles(0, M_PI / 3, 0);
	mvbl.UpdateTransforms();
	
	Model2World(10, 0, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5);
	EXPECT_FLOAT_EQ(result[1], 0);
	EXPECT_FLOAT_EQ(result[2], -10 * sqrt(3) / 2);

	Model2World(0, 0, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 30 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[1], 0);
	EXPECT_FLOAT_EQ(result[2], 15);

	mvbl.setAngles(0, 0, M_PI / 3);
	mvbl.UpdateTransforms();

	Model2World(10, 0, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5);
	EXPECT_FLOAT_EQ(result[1], 10 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[2], 0);

	Model2World(0, 20, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], - 20 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[1], 10);
	EXPECT_FLOAT_EQ(result[2], 0);

	mvbl.setAngles(M_PI / 3, 0, 0);
	mvbl.UpdateTransforms();

	Model2World(0, 20, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 0);
	EXPECT_FLOAT_EQ(result[1], 10);
	EXPECT_FLOAT_EQ(result[2], 20 * sqrt(3) / 2);

	Model2World(0, 0, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 0);
	EXPECT_FLOAT_EQ(result[1], -30 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[2], 15);
}
TEST(Transforms, Check2DVecRotations) 
{

	tTransformChain mvbl;
	float result[3];

	mvbl.setLocation(0, 0, 0);
	mvbl.setAngles(0, M_PI / 3, 0);
	mvbl.UpdateTransforms();

	Model2World(10, 0, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5 + 30 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[1], 0);
	EXPECT_FLOAT_EQ(result[2], -10 * sqrt(3) / 2 + 15);

	mvbl.setAngles(0, 0, M_PI / 3);
	mvbl.UpdateTransforms();

	Model2World(10, 20, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5 - 20 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[1], 10 * sqrt(3) / 2+ 10);
	EXPECT_FLOAT_EQ(result[2], 0);

	

	mvbl.setAngles(M_PI / 3, 0, 0);
	mvbl.UpdateTransforms();

	Model2World(0, 20, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 0);
	EXPECT_FLOAT_EQ(result[1], 10 - 30 * sqrt(3) / 2);
	EXPECT_FLOAT_EQ(result[2], 20 * sqrt(3) / 2+ 15);
}
TEST(Transforms, CheckTransformationChain) 
{

	tTransformChain mvbl;
	float result[3];

	mvbl.setScale(2, 1, 3);
	mvbl.setLocation(7, 11, 19);
	mvbl.setAngles(0, M_PI / 3, 0);
	mvbl.UpdateTransforms();

	Model2World(5, 0, 10, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5 + 30 * sqrt(3) / 2 + 7);
	EXPECT_FLOAT_EQ(result[1], 0 + 11);
	EXPECT_FLOAT_EQ(result[2], -10 * sqrt(3) / 2 + 15 + 19);
}
TEST(Transforms, CheckInverseTransformationChain) {

	tTransformChain mvbl;
	float result[3];

	mvbl.setScale(1, 1, 1);
	mvbl.setLocation(18, 0, 10);
	mvbl.setAngles(0, -M_PI / 6, 0);
	mvbl.UpdateTransforms();

	World2Model(20, 0, 10, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 2*sqrt(3)/2);
	EXPECT_FLOAT_EQ(result[1], 0);
	EXPECT_FLOAT_EQ(result[2], -1);

	mvbl.setScale(1, 1, 1);
	mvbl.setLocation(18, 0, 10);
	mvbl.setAngles(0, -M_PI / 4, 0);
	mvbl.UpdateTransforms();

	World2Model(20, 0, 10, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 2 * sqrt(2) / 2);
	EXPECT_FLOAT_EQ(result[1], 0);
	EXPECT_FLOAT_EQ(result[2], - 2 * sqrt(2) / 2);
}

