#pragma once
#include "imageResourceItem.h"

#define dir "../Content/img/templates/"

class ImageResources
{
public:
	String directory = dir;
	imageResourceItem imgGameLogo = imageResourceItem(directory + "gameLogo.bmp");

};

