#pragma once

#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "ORiUtils.h"
#include "A_Star.h"
#include <vector>

using namespace std;
using namespace cv;

struct mapNode
{
public:
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
	int type;
	Navigator(){};

	bool pathFind()
	{

	};


	vector<Point> doPathFinding(const deque<deque<MapTile>>& gridMap, Point myPosition)
	{
		vector<mapNode> matchedPoints = getDestinationCell(gridMap, myPosition);

		while (matchedPoints.size() > 0)
		{
			vector<Point> route = Apathfinder->doPathdinding(myPosition.x, myPosition.y, matchedPoints.back().possition.x, matchedPoints.back().possition.y, gridMap);
			if (route.size() > 0)
			{
				drawMap(gridMap, route, "Path");
				return route;
			}
			else
				matchedPoints.pop_back();
		}
		return vector<Point>();
	}


private:
	InputEmulator inputEmulator = InputEmulator();
	vector<mapNode> getDestinationCell(const deque<deque<MapTile>> & gridMap, Point myPosition)
	{
		vector<mapNode> matchedNodes;

		for (int row = 0; row < gridMap.size(); row++)
			for (int col = 0; col < gridMap.at(row).size(); col++)
				if (gridMap.at(row).at(col).getTimeCount() >= 0)
					matchedNodes.emplace_back(Point(row, col), gridMap.at(row).at(col).getTimeCount());

		std::sort(matchedNodes.begin(), matchedNodes.end(), more_than_mapNode());
		
		return matchedNodes;
	};

	void drawMap(const deque<deque<MapTile>> & map, vector<Point> & path,const string windowName) const
	{
		Mat mapImg(map.size(), map.front().size(), CV_8UC3);
		for (int row = 0; row < map.size(); row++)
			for (int col = 0; col < map.front().size(); col++)
			{
				mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col).mapElement->color;
			}


		for (int i = 0; i < path.size(); i++)
			mapImg.at<Vec3b>(Point(path.at(i).y, path.at(i).x)) = Vec3b::all(255);
		//cv::resize(mapImg, mapImg, cv::Size(), 6, 6, INTER_NEAREST);
		imshow(windowName, mapImg);
	};
};