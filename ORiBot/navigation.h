#pragma once

#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "A_Star.h"
#include <vector>

using namespace std;
using namespace cv;

class Navigator
{
public:
	A_Star *Apathfinder = new A_Star();
	shared_ptr<Point> globalPos;
	int type;
	Navigator(/*shared_ptr<Point> pGlobalPos*/)
	{
		//globalPos = pGlobalPos;
	};

	vector<Point> getDestinationCell(const deque<deque<MapTile>>& gridMap, Point myPosition) const
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

		if (matchedPoints.size() > 0)
			Apathfinder->doPathdindingB(myPosition.y, myPosition.x, matchedPoints.front().y, matchedPoints.front().x, gridMap);



		return matchedPoints;
	}
private:
};