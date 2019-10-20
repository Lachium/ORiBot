#pragma once

#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "A_Star.h"
#include <vector>

using namespace std;
using namespace cv;

struct mapNode
{
	Point possition;
	int timeScore;
	mapNode(Point possition, int timeScore) :possition(possition), timeScore(timeScore) {}
	mapNode() {};
};

struct more_than_mapNode
{
	inline bool operator() (const mapNode& struct1, const mapNode& struct2)
	{
		return (struct1.timeScore > struct2.timeScore);
	}
};

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
	bool doPathFinding(const deque<deque<MapTile>>& gridMap, Point myPosition)
	{
		vector<mapNode> matchedPoints = getDestinationCell(gridMap, myPosition);

		while (matchedPoints.size() > 0)
		{
			if (Apathfinder->doPathdinding(myPosition.x, myPosition.y, matchedPoints.back().possition.x, matchedPoints.back().possition.y, gridMap))
				return true;
			else
				matchedPoints.pop_back();
		}
		return false;
	}

	static double getDistance(Point pointA, Point pointB)
	{
		return double(sqrt((pointA.x * pointB.x) + (pointA.y + pointB.y)));
	}
private:

	vector<mapNode> getDestinationCell(const deque<deque<MapTile>>& gridMap, Point myPosition)
	{
		vector<mapNode> matchedNodes;

		for (int row = 0; row < gridMap.size(); row++)
			for (int col = 0; col < gridMap.at(row).size(); col++)
				if (gridMap.at(row).at(col).getTimeCount() >= 0)
					matchedNodes.emplace_back(Point(row, col), gridMap.at(row).at(col).getTimeCount());

		std::sort(matchedNodes.begin(), matchedNodes.end(), more_than_mapNode());
		
		return matchedNodes;
	};

};