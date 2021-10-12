#include "pch.h"
#include <iostream>
#include "../Core/movable.h"

TEST(Movables, CheckTranslations) {

	tMovable mvbl;
	float result[3];
	Model2World(10, 20, 30, &mvbl, result);
	
	
	EXPECT_FLOAT_EQ(result[0], 10);
	EXPECT_FLOAT_EQ(result[1], 20);
	EXPECT_FLOAT_EQ(result[2], 30);

	mvbl.setLocation(10, 10, 10);
	mvbl.UpdateModel2WorldMatrix();
	Model2World(10, 20, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 20);
	EXPECT_FLOAT_EQ(result[1], 30);
	EXPECT_FLOAT_EQ(result[2], 40);

	mvbl.translate(-7, 32, 81);
	mvbl.UpdateModel2WorldMatrix();
	Model2World(10, 20, 30, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 13);
	EXPECT_FLOAT_EQ(result[1], 62);
	EXPECT_FLOAT_EQ(result[2], 121);

}

TEST(Movables, CheckRotations) {

	tMovable mvbl;
	float result[3];

	mvbl.setLocation(0, 0, 0);
	mvbl.setAngles(M_PI / 3, 0, 0);
	mvbl.UpdateModel2WorldMatrix();
	Model2World(10, 20, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5);
	EXPECT_FLOAT_EQ(result[1], 20);
	EXPECT_FLOAT_EQ(result[2], -10 * sqrt(3) / 2);

	mvbl.setAngles(M_PI / 3, 0, 0);
	mvbl.UpdateModel2WorldMatrix();
	Model2World(10, 20, 0, &mvbl, result);
	EXPECT_FLOAT_EQ(result[0], 5);
	EXPECT_FLOAT_EQ(result[1], 20);
	EXPECT_FLOAT_EQ(result[2], -10 * sqrt(3) / 2);

}

