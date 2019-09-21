#pragma once

#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include <map>

using namespace std;
using namespace cv;

class MapElement
{
public:
	string name;
	Vec3b color;
	int mapCount = 0;
	MapElement(string qName, Vec3b qColor)
	{
		name = qName;
		color = qColor;
	};

private:
};

struct Vec3bKey {
	Vec3b color;
	Vec3bKey(int b, int g, int r)
	{
		color = Vec3b(b, g, r);
	};
	Vec3bKey(Vec3b colorT)
	{
		color = colorT;
	};

	// We compare Test objects by their ids. 
	bool operator<(const Vec3bKey& colorT) const
	{ 
		//cout << ((int)this->color[0] + (int)this->color[1] * 100 + (int)this->color[2] * 1000) << "  " << ((int)colorT.color[0] + (int)colorT.color[1] * 100 + (int)colorT.color[2] * 1000) << endl;
		if (abs((int)this->color[0] - (int)colorT.color[0]) < 8
			&& abs((int)this->color[1] - (int)colorT.color[1]) < 8
			&& abs((int)this->color[2] - (int)colorT.color[2]) < 8)
			return false;

		return (((int)this->color[0] + (int)this->color[1] * 100 + (int)this->color[2] * 1000)
			< ((int)colorT.color[0] + (int)colorT.color[1] * 100 + (int)colorT.color[2] * 1000));
	}
};

class MapElementCollection
{
public:
	map<Vec3bKey, MapElement> mapElements;

	MapElementCollection()
	{
		addElement(Vec3bKey(0, 0, 0), MapElement("Black", Vec3b(0, 0, 0)));
		addElement(Vec3bKey(255, 0, 0), MapElement("BlockedA", Vec3b(255, 0, 0)));
		addElement(Vec3bKey(255, 0, 255), MapElement("BlockedB", Vec3b(255, 0, 255)));
		addElement(Vec3bKey(0, 255, 0), MapElement("WalkableA", Vec3b(0, 255, 0)));
		addElement(Vec3bKey(0, 239, 0), MapElement("WalkableB", Vec3b(0, 239, 0)));
		addElement(Vec3bKey(0, 74, 255), MapElement("WalkableC", Vec3b(0, 74, 255)));
		addElement(Vec3bKey(0, 173, 255), MapElement("WalkableD", Vec3b(0, 173, 255)));
		addElement(Vec3bKey(255, 255, 0), MapElement("Water", Vec3b(255, 255, 0)));

		addElement(Vec3bKey(15, 0, 0), MapElement("mandragora", Vec3b(15, 0, 0)));//
		addElement(Vec3bKey(24, 0, 0), MapElement("poporing", Vec3b(24, 0, 0)));
		addElement(Vec3bKey(41, 0, 0), MapElement("smokie", Vec3b(41, 0, 0)));
		addElement(Vec3bKey(57, 0, 0), MapElement("bigfoot", Vec3b(57, 0, 0)));//
		addElement(Vec3bKey(71, 0, 0), MapElement("caramel", Vec3b(71, 0, 0)));
		addElement(Vec3bKey(90, 0, 0), MapElement("dustiness", Vec3b(90, 0, 0)));//
	};

	void addElement(Vec3bKey color, MapElement mob)
	{
		mapElements.insert({ color, mob });
	};
	MapElement searchMapElementByColor(Vec3b color)
	{
		if (mapElements.count(Vec3bKey(color)))
		{
			mapElements.at(color).mapCount++;
			return mapElements.at(Vec3bKey(color));
		}
		
		return mapElements.at(Vec3bKey(Vec3bKey(0, 0, 0)));
	};

	string getMapElementNameByColor(Vec3b color)
	{
		if (mapElements.count(color))
		{
			mapElements.at(color).mapCount++;
			return mapElements.at(color).name;
		}
		return "Black";
	};

	void clearCounts()
	{
		for (auto& key : mapElements)
			key.second.mapCount = 0;

	}
private:
};