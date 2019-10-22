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
	MapElement* mapElement;

	MapTile(MapElement* pMapElement, const int pTimeScore)
	{
		mapElement = pMapElement;
		timeScore = pTimeScore;
	};

	int getTimeCount() const
	{
		return timeScore;
	}

	void setTimeCount(const int pTimeScore)
	{
		timeScore = pTimeScore;
	}

private:
	int timeScore;
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
