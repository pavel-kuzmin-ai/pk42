#include "pch.h"
#include <iostream>
#include "../Core/quadtree.h"
#include "../Core/clock.h"
#include <unordered_set>
#include <cstdlib>

TEST(QueryQuadTree, CeckClosestSmall) {

	int field[4]{ 0,0,100,100 };	
	int data[] = {25, 75, 75, 75, 60, 30, 80, 10 };
	initTree(field, data, 4);
	buildQuadTree();

	int x = 60;
	int y = 10;
	int res = searchLinear(x, y);
	EXPECT_EQ(findClosest(x, y), res);
}

TEST(QueryQuadTree, CeckClosestMed) {

	int field[4]{ 0,0,100,100 };
	int data[] = { 20, 60, 30, 60, 20, 70, 40, 75, 25, 85, 70,80,80,80,80,70,60,10,80,30 };
	initTree(field, data, 10);
	buildQuadTree();

	int x = 30;
	int y = 20;
	int res = searchLinear(x, y);
	EXPECT_EQ(findClosest(x, y), res);

	
}

struct pair_hash {
	inline std::size_t operator()(const std::pair<int, int> & v) const {
		return v.first * 31 + v.second;
	}
};

/*
TEST(QueryQuadTree, SpeedTest) {

	int max_size = 255;
	int field[4]{ 0,0,max_size,max_size };
	

	std::unordered_set<std::pair<int, int>, pair_hash> my_points;

	int num_points = 16*16*4;
	for (int i = 0; i < num_points;i++)
	{
		int x = rand() % max_size;
		int y = rand() % max_size;
		my_points.insert({ x,y });
	}
	
	num_points = (int)my_points.size();
	int* data;
	data = new int[num_points*2];

	int i = 0;
	for (auto point : my_points)
	{
		data[i * 2] = point.first;
		data[i * 2 + 1] = point.second;
		i++;
	}

	initTree(field, data, num_points);
	buildQuadTree();

	const int num_q = 250*240;
	int qPoints[num_q * 2];
	for (int i = 0; i < num_q; i++)
	{
		qPoints[i * 2] = rand() % max_size;
		qPoints[i * 2 + 1] = rand() % max_size;
	}
	
	tClock clockCur, clockPrev;
	clockCur.init();
	clockPrev = clockCur;
	clockCur.startMeasure();
	
	float dt1 = 0;
	for (int i = 0; i < num_q; i++)
	{
		int x = qPoints[i * 2];
		int y = qPoints[i * 2 + 1];
		int resLinear = searchLinear(x, y);
		clockCur.checkAndSwapMeasure();
		dt1 += clockCur.calcDeltaSeconds(clockPrev);
		clockPrev = clockCur;

	}
		
	
	
	tClock clockCur2, clockPrev2;
	clockCur2.init();
	clockPrev2 = clockCur2;
	clockCur2.startMeasure();
	float dt2 = 0;
	for (int i = 0; i < num_q; i++)
	{
		int x = qPoints[i * 2];
		int y = qPoints[i * 2 + 1];
		int resTree = findClosest(x, y);

		clockCur2.checkAndSwapMeasure();
		dt2 += clockCur2.calcDeltaSeconds(clockPrev2);
		clockPrev2 = clockCur2;
	}
	
	
	std::cout << dt1 <<" "<< dt2 << '\n';
	
	EXPECT_EQ(1, 2);
}*/