#pragma once
#include "imageResourceItem.h"

#define dir "../Content/img/"

class ImageResources
{
public:
	String directory = dir;
	imageResourceItem imgGameLogo = imageResourceItem(directory + "templates/gameLogo.bmp");

};

