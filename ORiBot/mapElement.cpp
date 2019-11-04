#include "mapElement.h"

const map<Vec3bKey, MapElement*> MapElementCollection::mapElements = initMapElementCollection();


MapElement* MapElementCollection::searchMapElementByColor(Vec3b& color)
{
	if (mapElements.count(Vec3bKey(color)))
	{
		return mapElements.at(Vec3bKey(color));
	}

	return mapElements.at(Vec3bKey(Vec3bKey(255, 255, 255)));
};

MapElement* MapElementCollection::searchMapElementByColor(int blue, int green, int red)
{
	if (mapElements.count(Vec3bKey(blue, green, red)))
	{
		return mapElements.at(Vec3bKey(blue, green, red));
	}

	return mapElements.at(Vec3bKey(Vec3bKey(255, 255, 255)));
};

int MapElementCollection::getMapElementNameByColor(Vec3b color)
{
	if (mapElements.count(color))
	{
		return mapElements.at(color)->name;
	}
	return MapElementEnums::Unidentified;
};