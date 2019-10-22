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
	static const map<Vec3bKey, MapElement*> mapElements;
	//Functions
	static void draw(deque<deque<MapTile>>& map, const string windowName, const int resize = 1);
	static MapElement* searchMapElementByColor(Vec3b& color);
	static MapElement* searchMapElementByColor(int blue, int green, int red);
	static string getMapElementNameByColor(Vec3b color);


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

	static const map<Vec3bKey, MapElement*> initMapElementCollection()
	{
		map<Vec3bKey, MapElement*> thisMapElements;

		//  -- World Search Color -- Name -- RefColour -- Type --
		thisMapElements.insert({ Vec3bKey(0, 255, 255), new MapElement("Unseen", Vec3b(0, 255, 255), 3) });
		thisMapElements.insert({ Vec3bKey(255, 255, 255), new MapElement("Unknown", Vec3b(255, 255, 255), 2) });
		thisMapElements.insert({ Vec3bKey(0, 0, 0), new MapElement("Black", Vec3b(0, 0, 0), 2) });

		thisMapElements.insert({ Vec3bKey(79, 0, 0), new MapElement("BlockedA", Vec3b(79, 0, 0), 0) });
		thisMapElements.insert({ Vec3bKey(0, 0, 79), new MapElement("BlockedB", Vec3b(0, 0, 79), 0) });

		thisMapElements.insert({ Vec3bKey(0, 255, 0), new MapElement("WalkableA", Vec3b(0, 255, 0), 1) });
		thisMapElements.insert({ Vec3bKey(0, 239, 0), new MapElement("WalkableB", Vec3b(0, 255, 0), 1) });
		thisMapElements.insert({ Vec3bKey(0, 74, 255), new MapElement("WalkableC", Vec3b(0, 74, 255), 1) });
		thisMapElements.insert({ Vec3bKey(0, 173, 255), new MapElement("WalkableD", Vec3b(0, 173, 255), 1) });
		thisMapElements.insert({ Vec3bKey(255, 255, 0), new MapElement("Water", Vec3b(255, 255, 0), 1) });

		thisMapElements.insert({ Vec3bKey(15, 0, 0), new MapElement("mandragora", Vec3b(15, 0, 0), 2) });//
		thisMapElements.insert({ Vec3bKey(24, 0, 0), new MapElement("poporing", Vec3b(24, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(41, 0, 0), new MapElement("smokie", Vec3b(41, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(57, 0, 0), new MapElement("bigfoot", Vec3b(57, 0, 0), 2) });//
		thisMapElements.insert({ Vec3bKey(90, 0, 0), new MapElement("dustiness", Vec3b(90, 0, 0), 2) });//
		thisMapElements.insert({ Vec3bKey(107, 0, 0), new MapElement("caramel", Vec3b(107, 0, 0), 2) });

		return thisMapElements;
	};
};