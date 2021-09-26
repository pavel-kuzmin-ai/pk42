#ifndef QUADTREE_H
#define QUADTREE_H


#include <vector>

int* data;

struct tNode
{
	int childInd = 0;
	int childCount = 0;
};


std::vector<tNode> NodeArr;
int* rootBox;
tNode root{0, 0};


void initTree(int* _fieldSize, int* _data)
{
	rootBox = _fieldSize;
	data = _data;
	NodeArr.push_back(root);
};



int traverse(int x, int y)
{
	int currInd = 0;
	int currCount = NodeArr[currInd].childCount;
	int myBox
	while (currCount > 0)
	{

	}
	return 

};







#endif