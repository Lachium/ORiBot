#pragma once
#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "ORiUtils.h"
#include <vector>
#include <map>

using namespace std;
using namespace cv;


struct Entity {
	string name;
	Point pos;
	Entity(string pName, Point pPos)
	{
		name = pName;
		pos = pPos;
	};
};

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
		return (ORiUtils::getDistance(struct1.pos, dollPos) > ORiUtils::getDistance(struct2.pos, dollPos));
	}
};

class Environment
{
public:
	vector<Entity> mobiles;
	vector<Entity> ignores;
	Environment(vector<vector<MapElement*>>& itemMap, Point dollPos) {
		for (size_t row = 0; row < itemMap.size(); row++)
			for (size_t col = 0; col < itemMap.at(row).size(); col++)
				if (itemMap.at(row).at(col)->name == "Unknown" || itemMap.at(row).at(col)->name == "Black" || itemMap.at(row).at(col)->name == "shining_plant" || itemMap.at(row).at(col)->name == "green_plant" || itemMap.at(row).at(col)->name == "shadow")
					ignores.push_back(Entity(itemMap.at(row).at(col)->name, Point(row, col)));
				else if (itemMap.at(row).at(col)->type == 2)
					mobiles.push_back(Entity(itemMap.at(row).at(col)->name, Point(row, col)));


		std::sort(mobiles.begin(), mobiles.end(), order_entity_distance_to_Doll(dollPos));
	};
};
