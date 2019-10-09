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
	int type; 
	MapElement(const string qName, const Vec3b qColor, const int pType)
	{
		name = qName;
		color = qColor;
		type = pType;
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
		//cout << ((int)this->color[0] + (int)this->color[1] * 100 + (int)this->color[2] * 1000) << "  " << ((int)colorT.color[0] + (int)colorT.color[1] * 100 + (int)colorT.color[2] * 1000) <<  " ";
		if (abs((int)this->color[0] - (int)colorT.color[0]) < 10
			&& abs((int)this->color[1] - (int)colorT.color[1]) < 10
			&& abs((int)this->color[2] - (int)colorT.color[2]) < 10)
			return false;

		return (((int)this->color[0] + (int)this->color[1] * 100 + (int)this->color[2] * 1000)
			< ((int)colorT.color[0] + (int)colorT.color[1] * 100 + (int)colorT.color[2] * 1000));
	}
};

class MapElementCollection
{
public:
	map<Vec3bKey, const MapElement*> mapElements;

	MapElementCollection()
	{
		addElement(Vec3bKey(0, 255, 255), new MapElement("Unseen", Vec3b(0, 255, 255), 2));
		addElement(Vec3bKey(255, 255, 255), new MapElement("Unknown", Vec3b(255, 255, 255), 1));
		addElement(Vec3bKey(0, 0, 0), new MapElement("Black", Vec3b(0, 0, 0), 1));
		addElement(Vec3bKey(79, 0, 0), new MapElement("BlockedA", Vec3b(79, 0, 0), 0));
		addElement(Vec3bKey(0, 0, 79), new MapElement("BlockedB", Vec3b(0, 0, 79), 0));
		addElement(Vec3bKey(0, 255, 0), new MapElement("WalkableA", Vec3b(0, 255, 0), 0));
		addElement(Vec3bKey(0, 239, 0), new MapElement("WalkableB", Vec3b(0, 239, 0), 0));
		addElement(Vec3bKey(0, 74, 255), new MapElement("WalkableC", Vec3b(0, 74, 255), 0));
		addElement(Vec3bKey(0, 173, 255), new MapElement("WalkableD", Vec3b(0, 173, 255), 0));
		addElement(Vec3bKey(255, 255, 0), new MapElement("Water", Vec3b(255, 255, 0), 0));

		addElement(Vec3bKey(15, 0, 0), new MapElement("mandragora", Vec3b(15, 0, 0), 1));//
		addElement(Vec3bKey(24, 0, 0), new MapElement("poporing", Vec3b(24, 0, 0), 1));
		addElement(Vec3bKey(41, 0, 0), new MapElement("smokie", Vec3b(41, 0, 0), 1));
		addElement(Vec3bKey(57, 0, 0), new MapElement("bigfoot", Vec3b(57, 0, 0), 1));//
		addElement(Vec3bKey(90, 0, 0), new MapElement("dustiness", Vec3b(90, 0, 0), 1));//
		addElement(Vec3bKey(107, 0, 0), new MapElement("caramel", Vec3b(107, 0, 0), 1));
	};

	void addElement(Vec3bKey color, MapElement * mapElement)
	{
		mapElements.insert({ color, mapElement });
	};
	const MapElement * searchMapElementByColor(Vec3b color)
	{
		if (mapElements.count(Vec3bKey(color)))
		{
			return mapElements.at(Vec3bKey(color));
		}
		
		return mapElements.at(Vec3bKey(Vec3bKey(255, 255, 255)));
	};

	string getMapElementNameByColor(Vec3b color)
	{
		if (mapElements.count(color))
		{
			return mapElements.at(color)->name;
		}
		return "Unknown";
	};
private:
};