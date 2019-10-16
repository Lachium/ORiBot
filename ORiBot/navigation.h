#pragma once

#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include <vector>

using namespace std;
using namespace cv;

class Navigator
{
public:
	shared_ptr<Point> globalPos;
	int type;
	Navigator(/*shared_ptr<Point> pGlobalPos*/)
	{
		//globalPos = pGlobalPos;
	};

	vector<Point> getDestinationCell(const deque<deque<MapTile>>& gridMap) const
	{
		int maxTimeCountRead = INT_MAX;
		vector<Point> matchedPoints;

		for (int row = 0; row < gridMap.size(); row++)
		{
			for (int col = 0; col < gridMap.at(row).size(); col++)
			{
				if (gridMap.at(row).at(col).getTimeCount() <= maxTimeCountRead && gridMap.at(row).at(col).getTimeCount() >= 0)
				{
					if (gridMap.at(row).at(col).getTimeCount() < maxTimeCountRead)
					{
						maxTimeCountRead = gridMap.at(row).at(col).getTimeCount();
						matchedPoints.clear();
					}
					matchedPoints.push_back(Point(row, col));
				}
			}
		}
		return matchedPoints;
	}
private:
};