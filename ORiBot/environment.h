#pragma once
#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "ORiUtils.h"
#include <vector>
#include <map>

using namespace std;
using namespace cv;


struct Mobile {
	string name;
	Point pos;
	Mobile(string pName, Point pPos)
	{
		name = pName;
		pos = pPos;
	};
};

class order_Mobile_distance_to_Doll
{
public:
	Point dollPos;
	order_Mobile_distance_to_Doll(Point pDollPos)
	{
		dollPos = pDollPos;
	};
	inline bool operator() (const Mobile& struct1, const Mobile& struct2)
	{
		return (ORiUtils::getDistance(struct1.pos, dollPos) > ORiUtils::getDistance(struct2.pos, dollPos));
	}
};

class Environment
{
public:
	vector<Mobile> mobiles;
	Environment(vector<vector<MapElement*>> &itemMap, Point dollPos) {
		for (size_t row = 0; row < itemMap.size(); row++)
			for (size_t col = 0; col < itemMap.at(row).size(); col++)
				if (itemMap.at(row).at(col)->type == 2 && itemMap.at(row).at(col)->name != "Unknown" && itemMap.at(row).at(col)->name != "Black")
					mobiles.push_back(Mobile(itemMap.at(row).at(col)->name, Point(row,col)));


		std::sort(mobiles.begin(), mobiles.end(), order_Mobile_distance_to_Doll(dollPos));
	};
};
