#pragma once
#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "ORiUtils.h"
#include "entity.h"
#include <vector>
#include <map>

using namespace std;
using namespace cv;

class order_entity_distance_to_Doll
{
public:
	Point dollPos;
	order_entity_distance_to_Doll(Point pDollPos)
	{
		dollPos = pDollPos;
	};
	inline bool operator() (const Entity& struct1, const Entity& struct2)
	{
		if (Doll_Actions::getPriority(struct1.name) == Doll_Actions::getPriority(struct2.name))
			return (ORiUtils::getDistance(struct1.pos, dollPos) > ORiUtils::getDistance(struct2.pos, dollPos));
		else
			return (Doll_Actions::getPriority(struct1.name) < Doll_Actions::getPriority(struct2.name));
	}
};

class Environment
{
public:
	vector<Entity> mobiles;
	vector<Entity> ignores;
	Environment(vector<vector<MapElement*>>& itemMap) {

		Point dollPos = ORiUtils::getDollDefaultPos();

		for (size_t row = 0; row < itemMap.size(); row++)
			for (size_t col = 0; col < itemMap.at(row).size(); col++)
				if (itemMap.at(row).at(col)->name == Unknown || itemMap.at(row).at(col)->name == Unidentified || itemMap.at(row).at(col)->name == Black || itemMap.at(row).at(col)->name == Shadow)
					ignores.push_back(Entity(itemMap.at(row).at(col)->name, Point(row, col)));
				else if (itemMap.at(row).at(col)->type == 2)\
				{ 
					auto a = BresenhamLine(dollPos.x, dollPos.y, row, col);
					int aname = itemMap.at(row).at(col)->name;
					if (hasLineOfSite(BresenhamLine(dollPos.x, dollPos.y, row, col), itemMap))
					{
						mobiles.push_back(Entity(itemMap.at(row).at(col)->name, Point(row, col)));
					}
				}

		std::sort(mobiles.begin(), mobiles.end(), order_entity_distance_to_Doll(dollPos));
	};

	bool hasLineOfSite(const vector<Point> BresenhamLine, const vector<vector<MapElement*>>& itemMap)
	{
		for (Point BresenhamPoint : BresenhamLine)
				if (itemMap.at(BresenhamPoint.x).at(BresenhamPoint.y)->name == BlockedA)
					return false;

		return true;
	}

	// Returns the list of points from (x0, y0) to (x1, y1)
	vector<Point> BresenhamLine(int x0, int y0, int x1, int y1) {
		// Optimization: it would be preferable to calculate in
		// advance the size of "result" and to use a fixed-size array
		// instead of a list.
		vector<Point> result;

		auto swap = [](int& A, int& B) { int T = A; A = B; B = T;};

		bool steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep) {
			swap(x0, y0);
			swap(x1, y1);
		}
		if (x0 > x1) {
			swap(x0, x1);
			swap(y0, y1);
		}

		int deltax = x1 - x0;
		int deltay = abs(y1 - y0);
		int error = 0;
		int ystep;
		int y = y0;
		if (y0 < y1) ystep = 1; else ystep = -1;
		for (int x = x0; x <= x1; x++) {
			if (steep) result.push_back(Point(y, x));
			else result.push_back(Point(x, y));
			error += deltay;
			if (2 * error >= deltax) {
				y += ystep;
				error -= deltax;
			}
		}

		return result;
	}
};
