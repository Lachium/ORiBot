#pragma once

#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include <map>

using namespace std;
using namespace cv;

static enum MapElementEnums { Unseen, Unidentified, Shadow, Black, BlockedA, BlockedB, WalkableA, WalkableB, WalkableC, WalkableD, Water, mandragora, poporing, smokie, bigfoot, dustiness, caramel, green_plant, shining_plant, sasquatch, garm_baby_MVP, garm_MVP, marin, jakk,pecopeco, peco_egg, picky, yellow_plant, ant_egg, drops};


class MapElement
{
public:
	int name;
	Vec3b color;
	int type;
	MapElement(int qName, const Vec3b qColor, const int pType)
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
	MapElement* mapElement;
	int timeScore;

	MapTile(MapElement* pMapElement, const int pTimeScore)
	{
		mapElement = pMapElement;
		timeScore = pTimeScore;
	};

	int getTimeCount() const
	{
		return timeScore;
	};

	void setTimeCount(int pTimeScore)
	{
		timeScore = pTimeScore;
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
	Vec3bKey(Vec3b& colorT)
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

static class MapElementCollection
{
public:
	static const map<Vec3bKey, MapElement*> mapElements;
	static MapElement* searchMapElementByColor(Vec3b& color);
	static MapElement* searchMapElementByColor(int blue, int green, int red);
	static int getMapElementNameByColor(Vec3b color);
private:
	static const map<Vec3bKey, MapElement*> initMapElementCollection()
	{
		map<Vec3bKey, MapElement*> thisMapElements;

		//  -- World Search Color -- Name -- RefColour -- Type --
		thisMapElements.insert({ Vec3bKey(0, 255, 255), new MapElement(MapElementEnums::Unseen, Vec3b(0, 255, 255), 3) });
		thisMapElements.insert({ Vec3bKey(255, 255, 255), new MapElement(MapElementEnums::Unidentified, Vec3b(255, 255, 255), 2) });
		thisMapElements.insert({ Vec3bKey(74, 74, 206), new MapElement(MapElementEnums::Shadow, Vec3b(74, 74, 206), 2) });
		thisMapElements.insert({ Vec3bKey(0, 0, 0), new MapElement(MapElementEnums::Black, Vec3b(0, 0, 0), 2) });

		thisMapElements.insert({ Vec3bKey(255, 107, 181), new MapElement(MapElementEnums::BlockedA, Vec3b(255, 107, 181), 0) });
		thisMapElements.insert({ Vec3bKey(0, 0, 74), new MapElement(MapElementEnums::BlockedB, Vec3b(0, 0, 74), 0) });

		thisMapElements.insert({ Vec3bKey(0, 255, 0), new MapElement(MapElementEnums::WalkableA, Vec3b(0, 255, 0), 1) });
		thisMapElements.insert({ Vec3bKey(0, 239, 0), new MapElement(MapElementEnums::WalkableB, Vec3b(0, 255, 0), 1) });
		thisMapElements.insert({ Vec3bKey(0, 74, 255), new MapElement(MapElementEnums::WalkableC, Vec3b(0, 74, 255), 1) });
		thisMapElements.insert({ Vec3bKey(0, 173, 255), new MapElement(MapElementEnums::WalkableD, Vec3b(0, 173, 255), 1) });
		thisMapElements.insert({ Vec3bKey(255, 255, 0), new MapElement(MapElementEnums::Water, Vec3b(255, 255, 0), 1) });

		thisMapElements.insert({ Vec3bKey(15, 0, 0), new MapElement(MapElementEnums::mandragora, Vec3b(15, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(24, 0, 0), new MapElement(MapElementEnums::poporing, Vec3b(24, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(41, 0, 0), new MapElement(MapElementEnums::smokie, Vec3b(41, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(57, 0, 0), new MapElement(MapElementEnums::bigfoot, Vec3b(57, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(90, 0, 0), new MapElement(MapElementEnums::dustiness, Vec3b(90, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(107, 0, 0), new MapElement(MapElementEnums::caramel, Vec3b(107, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(123, 0, 0), new MapElement(MapElementEnums::green_plant, Vec3b(123, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(140, 0, 0), new MapElement(MapElementEnums::shining_plant, Vec3b(140, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(156, 0, 0), new MapElement(MapElementEnums::sasquatch, Vec3b(156, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(173, 0, 0), new MapElement(MapElementEnums::marin, Vec3b(173, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(0, 0, 173), new MapElement(MapElementEnums::garm_baby_MVP, Vec3b(0, 0, 173), 2) });
		thisMapElements.insert({ Vec3bKey(173, 0, 173), new MapElement(MapElementEnums::garm_MVP, Vec3b(173, 0, 173), 2) });
		thisMapElements.insert({ Vec3bKey(239, 156, 156), new MapElement(MapElementEnums::jakk, Vec3b(239, 156, 156), 2) });
		thisMapElements.insert({ Vec3bKey(189, 0, 0), new MapElement(MapElementEnums::pecopeco, Vec3b(189, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(206, 0, 0), new MapElement(MapElementEnums::peco_egg, Vec3b(206, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(223, 0, 0), new MapElement(MapElementEnums::picky, Vec3b(223, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(239, 0, 0), new MapElement(MapElementEnums::yellow_plant, Vec3b(239, 0, 0), 2) });
		thisMapElements.insert({ Vec3bKey(0, 24, 0), new MapElement(MapElementEnums::ant_egg, Vec3b(0, 24, 0), 2) });
		thisMapElements.insert({ Vec3bKey(24, 24, 0), new MapElement(MapElementEnums::drops, Vec3b(24, 24, 0), 2) });		
		
		return thisMapElements;
	};
};