#include "screenInterpreter.h"

bool ScreenInterpreter::screenToMapElements(Mat& screenImg, vector<vector<MapElement*>>& world, Point2f& cellOffset)
{
	ImageResource imgScreen = ImageResource(screenImg);

	if (cropToGameWindow(imgScreen))
	{
		isFullHealth = singleColorMatchingFast(*imgScreen.getColor(), *imageResourceCollection.imgFullHealth.getVec2D(), fullHealhPos, 0);
		if (!isFullHealth)
			isFullHealth = singleTemplateMatchingGrey(*imgScreen.getGray(), *imageResourceCollection.imgFullHealth.getGray(), 0.9999f, fullHealhPos);

		cellOffset = getGridBinOffset(imgScreen);
		world = calculateGridPixels(imgScreen, cellOffset);
		return !world.empty();
	}
	else
		ORiUtils::ConsoleLog("Could not Crop");

	return false;
}

bool ScreenInterpreter::singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, Point& matchPoint) const
{
	Mat mResult;
	matchTemplate(mInput, mTemplate, mResult, TM_CCOEFF_NORMED);
	threshold(mResult, mResult, Threshold, 1.0, THRESH_TOZERO);

	double minval, maxval;
	Point minloc, maxloc;
	minMaxLoc(mResult, &minval, &maxval, &minloc, &maxloc);

	if (maxval >= Threshold)
	{
		matchPoint = (maxloc);
		return true;
	}
	else
	{
		ORiUtils::ConsoleLog("No Match Found");
		return false;
	}
}

bool ScreenInterpreter::colorSearchSingleMap(Mat& colorImg, Vec3b color, Point& matchPoint) const
{
	for (int r = binHeight * 2; r < colorImg.rows - 1 - binHeight * 0; r++)
		for (int c = binWidth * 2; c < colorImg.cols - 1 - binWidth * 0; c++)
		{
			//4 channel image compare to 3 channel image
			if (colorImg.type() == 24)
			{
				const int offset = abs((colorImg.at<Vec4b>(Point(c, r))[0] - color[0])) +
					abs((colorImg.at<Vec4b>(Point(c, r))[1] - color[1])) +
					abs((colorImg.at<Vec4b>(Point(c, r))[2] - color[2]));

				if (offset < 1)
				{
					matchPoint = Point(c, r);
					return true;
				}
			}
			//3 channel image compare to 3 channel image
			else
			{
				const int offset = abs((colorImg.at<Vec3b>(Point(c, r))[0] - color[0])) +
					abs((colorImg.at<Vec3b>(Point(c, r))[1] - color[1])) +
					abs((colorImg.at<Vec3b>(Point(c, r))[2] - color[2]));

				if (offset == 0)
				{
					matchPoint = Point(c, r);
					return true;
				}
			}
		}
	ORiUtils::ConsoleLog("No Fast Color Find ");
	return false;
}

Point2f ScreenInterpreter::getGridBinOffset(ImageResource& img)
{
	Point firstTile;
	if (!colorSearchSingleMap(*img.getColor(), Vec3b(0, 255, 0), firstTile))
		return Point2f(0, 0);

	const int  exBinX = (int)(firstTile.x / (binWidth + (firstTile.x / 1280.0) * 0.567));
	const int  exBinY = (int)(firstTile.y / binHeight);

	const vector<vector<Point2f>>& expectedPoints = ORiUtils::cellPositionalMap;

	const int  xOffset = ((40 - xOffsetConst / 10.0) - (expectedPoints.at(exBinY).at(exBinX).x - firstTile.x));
	const int  yOffset = ((20 - yOffsetConst / 10.0) - (expectedPoints.at(exBinY).at(exBinX).y - firstTile.y));
	return Point2f(xOffset, yOffset);
}

bool ScreenInterpreter::cropToGameWindow(ImageResource& img)
{
	bool couldFindFast = singleColorMatchingFast(*img.getColor(), *imageResourceCollection.imgGameLogo.getVec2D(), gameLogoPos);
	if (couldFindFast)
		return img.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));

	bool couldFindSlow = singleTemplateMatchingGrey(*img.getGray(), *imageResourceCollection.imgGameLogo.getGray(), 0.9f, gameLogoPos);
	if (couldFindSlow)
		return img.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));

	return false;
}

bool ScreenInterpreter::singleColorMatchingFast(Mat& colorImg, vector<vector<Vec3b>>& cTemplate, Point& searhPoint, int MaxOffset) const
{
	if (((cTemplate.at(0).size() + searhPoint.x) >= colorImg.cols) || ((cTemplate.size() + searhPoint.y) >= colorImg.rows) || searhPoint.x < 0 || searhPoint.y < 0)
	{
		ORiUtils::ConsoleLog("Fast color search point out of bounds ");
		return false;
	}
	for (int r = 0; r < cTemplate.size(); r++)
		for (int c = 0; c < cTemplate.at(0).size(); c++)
		{
			//4 channel image compare to 3 channel image
			if (colorImg.type() == 24)
			{
				const int offset = abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[0] - (cTemplate.at(r).at(c)[0]))) +
					abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[1] - (cTemplate.at(r).at(c)[1]))) +
					abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[2] - (cTemplate.at(r).at(c)[2])));

				if (offset > MaxOffset)
				{
					ORiUtils::ConsoleLog("No Fast Color Found");
					return false;
				}
			}
			//3 channel image compare to 3 channel image
			else
			{
				const int offset = abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[0] - (cTemplate.at(r).at(c)[0]))) +
					abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[1] - (cTemplate.at(r).at(c)[1]))) +
					abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[2] - (cTemplate.at(r).at(c)[2])));
				if (offset != 0)
				{
					ORiUtils::ConsoleLog("No Fast Color Found");
					return false;
				}
			}
		}
	return true;
}

vector<vector<MapElement*>> ScreenInterpreter::calculateGridPixels(ImageResource& img, Point2f binOffsets)
{
	const int boarder = 1;
	const int rezize = 2;
	const vector<vector<Point2f>>& expectedPoints = ORiUtils::cellPositionalMap;
	vector<vector<MapElement*>> map;
	map.reserve(expectedPoints.size());
	for (int r = boarder; r < expectedPoints.size() - boarder; r++)
	{
		vector<MapElement*> mapLine;
		mapLine.reserve(expectedPoints.front().size());
		for (int c = boarder; c < expectedPoints.front().size() - boarder; c++)
		{
			vector<Vec3b> colors;
			colors.reserve((blockWidth - rezize * 2) * (blockHeight - rezize * 2));

			for (int x = rezize; x < blockWidth - rezize; x++)
				for (int y = rezize; y < blockHeight - rezize; y++)
				{
					const int xpos = expectedPoints.at(r).at(c).x + x + binOffsets.x;
					const int ypos = expectedPoints.at(r).at(c).y + y + binOffsets.y;
					if (!((xpos < 0 || ypos < 0 || xpos >= img.getColor()->cols || ypos >= img.getColor()->rows)))
					{
						const Vec4b color4b = img.getColor()->at<Vec4b>(Point(xpos, ypos));
						const Vec3b color = Vec3b((int)color4b[0], (int)color4b[1], (int)color4b[2]);
						if (!(color[0] == color[1] && color[1] == color[2]))
							colors.push_back(color);
					}
				}
			if (!colors.empty())
				mapLine.push_back(getMode(colors));
			else
				mapLine.push_back(MapElementCollection::searchMapElementByColor(0, 255, 255));
		}
		map.push_back(mapLine);
	}
	return map;
}

MapElement* ScreenInterpreter::getMode(vector<Vec3b>& colors)
{
	Vec3b& number = colors.front();
	Vec3b& mode = number;
	int count = 1;
	int countMode = 1;

	for (int i = 1; i < colors.size(); i++)
	{
		if (colors.at(i) == number)
			++count;
		else
		{
			if (count > countMode)
			{
				countMode = count;
				mode = number;
			}
			count = 1;
			number = colors.at(i);
		}
	}
	return MapElementCollection::searchMapElementByColor(mode);
}