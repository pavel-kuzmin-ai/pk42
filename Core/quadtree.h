#ifndef QUADTREE_H
#define QUADTREE_H


#include <vector>
#include <queue>

int* data;
int dataSize;

struct tNode
{
	int childInd = 0;
	int childCount = 0;
};


std::vector<tNode> NodeArr;
int* rootBox;
int nodesHead = 1;
tNode root{ nodesHead, 0};


void initTree(int* _fieldSize, int* _data, int _dataSize)
{
	rootBox = _fieldSize;
	data = _data;
	dataSize = _dataSize;
	NodeArr.clear();
	NodeArr.push_back(root);
};

inline int getQuadrantIdx(int x, int y, int bordX, int bordY)
{
	if ((x <  bordX) && (y >=  bordY)) return 0;
	if ((x >= bordX) && (y >=  bordY)) return 1;
	if ((x <  bordX) && (y <   bordY)) return 2;
	if ((x >= bordX) && (y <   bordY)) return 3;
	return -1;
}

inline int getChildCountByIdx(int idx)
{
	return NodeArr[idx].childCount;
}

inline void recalcBox(int indTo, int bordX, int bordY, int*prevBox)
{
	if (indTo == 0)
	{
		prevBox[2] = bordX;
		prevBox[1] = bordY;

		return;
	}

	if (indTo == 1)
	{
		prevBox[0] = bordX;
		prevBox[1] = bordY;

		return;
	}

	if (indTo == 2)
	{
		prevBox[2] = bordX;
		prevBox[3] = bordY;

		return;
	}

	if (indTo == 3)
	{
		prevBox[0] = bordX;
		prevBox[3] = bordY;

		return;
	}
}

int traverse(int x, int y, int myInd, int* myBox, bool findLeaf=true)
{
	int currInd = myInd;

	while (getChildCountByIdx(currInd)<0)
	{
		int bordX = (myBox[0] + myBox[2]) / 2;
		int bordY = (myBox[1] + myBox[3]) / 2;

		int childIdx = getQuadrantIdx(x, y, bordX, bordY);

		int nxtIdx = NodeArr[currInd].childInd + childIdx;
		if ((!findLeaf) && (NodeArr[nxtIdx].childCount >= 0))
		{ 
			break; 
		}
		recalcBox(childIdx, bordX, bordY, myBox);
		currInd = nxtIdx;

	}
	return currInd;
}


void splitNode(int NodeIdx, int bordX, int bordY)
{
	
	int cIdx = (int)NodeArr.size();
	NodeArr.push_back(tNode{ 0, 0 });
	NodeArr.push_back(tNode{ 0, 0 });
	NodeArr.push_back(tNode{ 0, 0 });
	NodeArr.push_back(tNode{ 0, 0 });
	
	int qIdx = getQuadrantIdx(data[NodeArr[NodeIdx].childInd], data[NodeArr[NodeIdx].childInd + 1], bordX, bordY);
	NodeArr[cIdx + qIdx].childCount = 1;
	NodeArr[cIdx + qIdx].childInd = NodeArr[NodeIdx].childInd;

	NodeArr[NodeIdx].childInd = cIdx;

	NodeArr[NodeIdx].childCount = -1;		
}

void insertPoint(int dataInd)
{
	int myBox[4];
	memcpy(myBox, rootBox, 4 * sizeof(int));
	int x = data[dataInd];
	int y = data[dataInd + 1];

	int idxToInsert = traverse(x, y, 0, myBox, true);
	
	while (NodeArr[idxToInsert].childCount > 0)
	{
		splitNode(idxToInsert, (myBox[0]+myBox[2]) / 2, (myBox[1] + myBox[3]) / 2);
		idxToInsert = traverse(x, y, idxToInsert, myBox, true);
	}

	NodeArr[idxToInsert].childCount = 1;
	NodeArr[idxToInsert].childInd = dataInd;
}

void buildQuadTree()
{
	for (int i = 0; i < dataSize; i++)
	{
		insertPoint(i*2);
	}
}

int findAnyPoint(int indx)
{
	for (int i = 0; i < 4; i++)
	{
		int cInd = NodeArr[indx].childInd + i;
		if (NodeArr[cInd].childCount > 0)
		{
			return NodeArr[cInd].childInd;
		}
		
		if (NodeArr[cInd].childCount == -1)
		{
			return findAnyPoint(cInd);
		}
		
	}
	return 0;
}

inline int calcSqDist(int x1, int y1, int x2, int y2)
{
	int dx = x1 - x2;
	int dy = y1 - y2;
	return dx * dx + dy * dy;
}

inline int pointSegment(int x, int x1, int x2)
{
	if ((x>=x1)&&(x<=x2)) return 0;
	if (x < x1) return x1 - x;
	if (x > x1) return x - x2;
	return 0;
}

int distBox(int x, int y, int l, int b, int r, int t)
{
	int dx = pointSegment(x, l, r);
	int dy = pointSegment(y, b, t);
	return dx * dx + dy * dy;
}

int distBox(int x, int y, int* box)
{	
	return distBox(x, y, box[0], box[1], box [2], box[3]);
}

struct tChildData
{
	int idx;
	int l;
	int b;
	int r;
	int t;
};

inline bool isQuadValid(int x, int y, int minDist, int l, int b, int r, int t)
{
	return distBox(x, y, l, b, r, t) < minDist;
}

int findClosest(int x, int y)
{
	int myBox[4];
	memcpy(myBox, rootBox, 4 * sizeof(int));
	int subtreeIdx = traverse(x, y, 0, myBox, false);
	
	int closest = findAnyPoint(subtreeIdx);
	int minDist = calcSqDist(x, y, data[closest], data[closest + 1]);
	
	std::queue<tChildData> toProcess;
	toProcess.push(tChildData{ 0, rootBox[0], rootBox[1], rootBox[2], rootBox[3] });
	while (!toProcess.empty())
	{
		tChildData currNode = toProcess.front();
		toProcess.pop();
		int currChildCount = NodeArr[currNode.idx].childCount;
		int currFirstCild = NodeArr[currNode.idx].childInd;
		if (currChildCount == 0) continue;
		
		
		if (currChildCount > 0)
		{
			for (int i = 0; i < currChildCount;i++) 
			{
				int cInd = currFirstCild + i;
				int distance = calcSqDist(x, y, data[cInd], data[cInd + 1]);
				if (distance < minDist)
				{
					minDist = distance;
					closest = currFirstCild + i;
				}
			}			
		}
		if (currChildCount < 0)
		{	
			int midX = (currNode.l + currNode.r) / 2;
			int midY = (currNode.b + currNode.t) / 2;

			if (isQuadValid(x, y, minDist, currNode.l, midY, midX, currNode.t))
			{
				toProcess.push(tChildData{ currFirstCild,     currNode.l, midY,       midX,       currNode.t });
			}

			if (isQuadValid(x, y, minDist, midX, midY, currNode.r, currNode.t))
			{
				toProcess.push(tChildData{ currFirstCild + 1, midX,       midY,       currNode.r, currNode.t });
			}
			
			if (isQuadValid(x, y, minDist, currNode.l, currNode.b, midX, midY))
			{
				toProcess.push(tChildData{ currFirstCild + 2, currNode.l, currNode.b, midX,       midY });
			}

			if (isQuadValid(x, y, minDist, midX, currNode.b, currNode.r, midY))
			{
				toProcess.push(tChildData{ currFirstCild + 3, midX,       currNode.b, currNode.r, midY });
			}
			
		}
	}
	return closest;
}

int searchLinear(int x, int y)
{
	int idx = -1;
	int closestDist = calcSqDist(x, y , data[0], data[1]);
	for (int i = 0; i < dataSize*2; i+=2)
	{
		int currDist = calcSqDist(x, y, data[i], data[i + 1]);
		if (currDist < closestDist)
		{
			idx = i;
			closestDist = currDist;
		}
			
	}
	return idx;
}







#endif