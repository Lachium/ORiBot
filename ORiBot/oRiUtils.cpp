#include "oRiUtils.h"

//staticStuff
const vector<vector<Point2f>> ORiUtils::cellPositionalMap = generateCellPositionalMap();
const map<Vec3bKey, MapElement*> ORiUtils::mapElements = initMapElementCollection();

void ORiUtils::draw(deque<deque<MapTile>>& image, const string windowName, const int resize)
{
	Mat mapImg(image.size(), image.front().size(), CV_8UC3);
	for (int row = 0; row < image.size(); row++)
		for (int col = 0; col < image.front().size(); col++)
		{
				mapImg.at<Vec3b>(Point(col, row)) = image.at(row).at(col).mapElement->color;
		}
	if(resize > 1)
	cv::resize(mapImg, mapImg, cv::Size(), resize, resize, INTER_NEAREST);

	imshow(windowName, mapImg);
};



MapElement* ORiUtils::searchMapElementByColor(Vec3b& color)
{
	if (mapElements.count(Vec3bKey(color)))
	{
		return mapElements.at(Vec3bKey(color));
	}

	return mapElements.at(Vec3bKey(Vec3bKey(255, 255, 255)));
};

MapElement* ORiUtils::searchMapElementByColor(int blue, int green, int red)
{
	if (mapElements.count(Vec3bKey(blue, green, red)))
	{
		return mapElements.at(Vec3bKey(blue, green, red));
	}

	return mapElements.at(Vec3bKey(Vec3bKey(255, 255, 255)));
};

string ORiUtils::getMapElementNameByColor(Vec3b color)
{
	if (mapElements.count(color))
	{
		return mapElements.at(color)->name;
	}
	return "Unknown";
};