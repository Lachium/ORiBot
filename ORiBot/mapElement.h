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


class MapTile
{
public:
	int timeScore = -1;
	MapElement * mapElement;

	MapTile(MapElement *pMapElement)
	{
		mapElement = pMapElement;
	};

private:
};


struct Vec3bKey {
	int blue, green, red;
	Vec3bKey(int b, int g, int r)
	{
		blue = b;
		green = g;
		red = r;
	};
	Vec3bKey(Vec3b &colorT)
	{
		blue = (int)colorT[0];
		green = (int)colorT[1];
		red = (int)colorT[2];
	};
	
	bool operator<(const Vec3bKey& colorT) const
	{ 
		if (abs(blue - colorT.blue) < 10
			&& abs(green - colorT.green) < 10
			&& abs(red - colorT.red) < 10)
			return false;

		return (blue + green * 100 + red * 1000)
			< (colorT.blue + colorT.green * 100 + colorT.red * 1000);
	}
};

class MapElementCollection
{
public:
	map<Vec3bKey, MapElement*> mapElements;

	MapElementCollection()
	{
		//  -- World Search Color -- Name -- RefColour -- Type --
		addElement(Vec3bKey(0, 255, 255), new MapElement("Unseen", Vec3b(0, 255, 255), 2));
		addElement(Vec3bKey(255, 255, 255), new MapElement("Unknown", Vec3b(255, 255, 255), 1));
		addElement(Vec3bKey(0, 0, 0), new MapElement("Black", Vec3b(0, 0, 0), 1));
		addElement(Vec3bKey(79, 0, 0), new MapElement("BlockedA", Vec3b(79, 0, 0), 0));
		addElement(Vec3bKey(0, 0, 79), new MapElement("BlockedB", Vec3b(0, 0, 79), 0));
		addElement(Vec3bKey(0, 255, 0), new MapElement("WalkableA", Vec3b(0, 255, 0), 0));
		addElement(Vec3bKey(0, 239, 0), new MapElement("WalkableB", Vec3b(0, 255, 0), 0));
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
	MapElement * searchMapElementByColor(Vec3b &color) 
	{
		if (mapElements.count(Vec3bKey(color)))
		{
			return mapElements.at(Vec3bKey(color));
		}
		
		return mapElements.at(Vec3bKey(Vec3bKey(255, 255, 255)));
	};

	MapElement* searchMapElementByColor(int blue, int green, int red)
	{
		if (mapElements.count(Vec3bKey(blue, green, red)))
		{
			return mapElements.at(Vec3bKey(blue, green, red));
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