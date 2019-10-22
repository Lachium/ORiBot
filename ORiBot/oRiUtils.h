#pragma once

#include<opencv2/opencv.hpp>
#include <windows.h>
#include "mapElement.h"

#define maxBinsX 47
#define maxBinsY 27

#define shiftA 331
#define shiftB 140
#define shiftC 904

#define binWidth 26.667
#define binHeight 26.667


using namespace std;
using namespace cv;

static class ORiUtils
{
public:
	ORiUtils() { };
	//Variables
	static const vector<vector<Point2f>> cellPositionalMap;
	//Functions
	static void draw(deque<deque<MapTile>>& map, const string windowName, const int resize = 1);


private:
	static const vector<vector<Point2f>> generateCellPositionalMap()
	{
		vector<vector<Point2f>> thisCellPositionalMap;
		for (int r = 0; r < maxBinsY; r++)
		{
			vector<Point2f> line;
			for (int c = 0; c < maxBinsX; c++)
			{
				const double xShift = ((-(maxBinsX * 0.5) + c) * (0.5 - shiftA / 1000.0)) + (-(maxBinsX * 0.5) + c) * ((-(maxBinsY * 0.5) + r)) * (shiftB / 10000.0);
				const double yShift = r * (1 - shiftC / 1000.0);
				line.push_back(Point2f((c * binWidth + xShift), (r * binHeight + yShift)));
			}
			thisCellPositionalMap.push_back(line);
		}
		return thisCellPositionalMap;
	}

};