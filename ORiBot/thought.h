#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include "mob.h"

using namespace std;
using namespace cv;

class Thought
{
	MapElementCollection mapElementCollection;
public:
	deque<deque<MapElement>> gridMap;
	Point lastGridPos = Point(-1,-1);

	Thought() {};
	void lookAtMapConents(Mat& world);
	void appendToMap(deque<deque<MapElement>> & mapPiece);
	MapElement innerConditions(const bool pPartCod, const bool gPartCod, const int pPartRow, const int pPartCol, const int gPartRow, const int gPartCol, deque<deque<MapElement>>& mapPiece, deque<deque<MapElement>>& gridMap);
	void drawMap(deque<deque<MapElement>>& map, string windowName);
	void pushToMap(int pushRow, int pushCol, int& foundRow, int& foundCol, MapElement mapElement);
};

