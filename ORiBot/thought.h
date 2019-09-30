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
	vector<vector<MapElement>> gridMap;

	Thought() {};
	void lookAtMapConents(Mat& world);
	void appendToMap(vector<vector<MapElement>> & mapPiece);
	MapElement innerConditions(const bool pPartCod, const bool gPartCod, const int pPartRow, const int pPartCol, const int gPartRow, const int gPartCol, vector<vector<MapElement>>& mapPiece, vector<vector<MapElement>>& gridMap);
	void drawMap(vector<vector<MapElement>>& map, string windowName);
};

