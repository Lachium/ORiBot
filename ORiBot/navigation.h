#pragma once

#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "ORiUtils.h"
#include "A_Star.h"
#include <vector>

using namespace std;
using namespace cv;

class MapNode
{
public:
	Point possition;
	MapTile* maptileRef;

	MapNode(Point pPossition, MapTile* pMaptileRef)
	{
		possition = pPossition;
		maptileRef = pMaptileRef;

	};
	//MapNode() {};
};

class order_mapNode_score_then_distance
{
public:
	order_mapNode_score_then_distance(Point pDollPos) { dollPos = pDollPos; };
	Point dollPos;
	inline bool operator() (MapNode& struct1, MapNode& struct2)
	{
		if (struct1.maptileRef->timeScore == struct2.maptileRef->timeScore)
			return (ORiUtils::getDistance(struct1.possition, dollPos) > ORiUtils::getDistance(struct2.possition, dollPos));
		else
			return (struct1.maptileRef->timeScore > struct2.maptileRef->timeScore);
	}
};

class Navigator
{
public:
	A_Star *Apathfinder = new A_Star();
	int type;
	Navigator(){};

	bool pathFind()
	{

	};


	vector<Point> doPathFinding(deque<deque<MapTile>>& gridMap, Point dollPos)
	{
		vector<MapNode> matchedPoints = getDestinationCell(gridMap, dollPos);

		while (matchedPoints.size() > 0)
		{
			vector<Point> route = Apathfinder->doPathdinding(dollPos.x, dollPos.y, matchedPoints.back().possition.x, matchedPoints.back().possition.y, gridMap);
			if (route.size() > 0)
			{
				drawMap(gridMap, route, "Path");
				return route;
			}
			else
			{
				matchedPoints.back().maptileRef->setTimeCount(matchedPoints.front().maptileRef->getTimeCount());
				matchedPoints.pop_back();
			}
		}
		return vector<Point>();
	}


private:
	InputEmulator inputEmulator = InputEmulator();
	vector<MapNode> getDestinationCell(deque<deque<MapTile>> & gridMap, Point dollPos)
	{
		vector<MapNode> matchedNodes;

		for (int row = 0; row < gridMap.size(); row++)
			for (int col = 0; col < gridMap.at(row).size(); col++)
				if (gridMap.at(row).at(col).getTimeCount() >= 0)
					 matchedNodes.emplace_back(Point(row, col), &gridMap.at(row).at(col));

		std::sort(matchedNodes.begin(), matchedNodes.end(), order_mapNode_score_then_distance(dollPos));
		
		return matchedNodes;
	};

	void drawMap(const deque<deque<MapTile>> & map, vector<Point> & path, const string windowName) const
	{
		Mat mapImg(map.size(), map.front().size(), CV_8UC3);
		for (int row = 0; row < map.size(); row++)
			for (int col = 0; col < map.front().size(); col++)
			{
				mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col).mapElement->color;
			}


		for (int i = 0; i < path.size(); i++)
			mapImg.at<Vec3b>(Point(path.at(i).y, path.at(i).x)) = Vec3b::all(255);
		//cv::resize(mapImg, mapImg, cv::Size(), 3, 3, INTER_NEAREST);
		imshow(windowName, mapImg);
	};
};