#include "screenInterpreter.h"

ScreenInterpreter::ScreenInterpreter()
{
	//Generate Cell Pixel Lookup Grid
	for (int r = 0; r < maxBinsY; r++)
	{
		vector<Point2f> line;
		for (int c = 0; c < maxBinsX; c++)
		{
			const double xShift = ((-(maxBinsX * 0.5) + c) * (0.5 - shiftA / 1000.0)) + (-(maxBinsX * 0.5) + c) * ((-(maxBinsY * 0.5) + r)) * (shiftB / 10000.0);
			const double yShift = r * (1 - shiftC / 1000.0);
			line.push_back(Point2f((c * binWidth + xShift), (r * binHeight + yShift)));
		}
		expectedPoints.push_back(line);
	}
}

bool ScreenInterpreter::screenToMapElements(Mat& screenImg, vector<vector<const MapElement*>>& world)
{
	ImageResource imgScreen = ImageResource(screenImg);

	if (cropToGameWindow(imgScreen))
	{
		Point2f offset = getGridBinOffset(imgScreen);
		world = getGridPixels(imgScreen, offset);
		return !world.empty();
	}
	else
		cout << "Could not Crop ";

	return false;
}

bool ScreenInterpreter::singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, Point& matchPoint)
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
		cout << "No Match Found ";
		return false;
	}
}

bool ScreenInterpreter::colorSearchSingleMap(Mat& colorImg, Vec3b color, Point& matchPoint)
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
	cout << "No Fast Color Find ";
	return false;
}

Point2f ScreenInterpreter::getGridBinOffset(ImageResource& img)
{
	Point firstTile;
	if (!colorSearchSingleMap(*img.getColor(), Vec3b(0, 255, 0), firstTile))
		return Point2f(0, 0);

	const int  exBinX = (int)(firstTile.x / (binWidth + (firstTile.x / 1280.0) * 0.567));
	const int  exBinY = (int)(firstTile.y / binHeight);
	const int  xOffset = ((40 - xOffsetConst / 10.0) - (expectedPoints.at(exBinY).at(exBinX).x - firstTile.x));
	const int  yOffset = ((20 - yOffsetConst / 10.0) - (expectedPoints.at(exBinY).at(exBinX).y - firstTile.y));

	/*for (int r = 0; r < expectedPoints.size(); r++)
	{
		int binWidthThis = (binWidth + (firstTile.x / 1280.0) * 0.567);
		for (int c = 0; c < expectedPoints.front().size(); c++)
		{
			rectangle(*img.getColor(), Point(expectedPoints.at(r).at(c).x + xOffset, expectedPoints.at(r).at(c).y + yOffset), Point(expectedPoints.at(r).at(c).x + blockWidth + xOffset, expectedPoints.at(r).at(c).y + blockHeight + yOffset), cv::Scalar(255, 100, 0));
	//		rectangle(*img.getColor(), Point(expectedPoints.at(r).at(c).x + xOffset - binWidthThis / 2, expectedPoints.at(r).at(c).y + yOffset - binHeight / 2), Point(expectedPoints.at(r).at(c).x + xOffset + binWidthThis / 2, expectedPoints.at(r).at(c).y + yOffset + binHeight / 2), cv::Scalar(255, 255, 100), 0);
		}
	}*/

	//Point start = Point(expectedPoints.at(exBinY).at(exBinX).x + xOffset , +expectedPoints.at(exBinY).at(exBinX).y + yOffset);
	//Point end = Point(start.x + blockWidth, start.y + blockHeight);
	//rectangle(*img.getColor(), start, end, cv::Scalar(255, 100, 255), 5);
	//Point startF = Point(firstTile.x,firstTile.y);
	//Point endF = Point(startF.x + 3, startF.y + 3);
	//rectangle(*img.getColor(), startF, endF, cv::Scalar(100, 255, 255), 5);
	return Point2f(xOffset, yOffset);
}

bool ScreenInterpreter::cropToGameWindow(ImageResource& img)
{

	vector<vector<Vec3b>> cVecOutput;
	imageTo2dCollorVec(*imageResourceCollection.imgGameLogo.getColor(), cVecOutput, Point(2, 2));

	if (singleColorMatchingFast(*img.getColor(), cVecOutput, gameLogoPos))
		return img.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));
	else {
		if (singleTemplateMatchingGrey(*img.getGray(), *imageResourceCollection.imgGameLogo.getGray(), 0.9f, gameLogoPos))
		{
			return img.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));
		}
		else
			return false;
	}

	return false;
}

bool ScreenInterpreter::singleColorMatchingFast(Mat& colorImg, vector<vector<Vec3b>>& cTemplate, Point& searhPoint)
{
	if (((cTemplate.at(0).size() + searhPoint.x) >= colorImg.cols) || ((cTemplate.size() + searhPoint.y) >= colorImg.rows) || searhPoint.x < 0 || searhPoint.y < 0)
	{
		cout << "Fast color search point out of bounds ";
		return false;
	}
	for (int r = 0; r < cTemplate.size(); r++)
		for (int c = 0; c < cTemplate.at(0).size(); c++)
		{
			//4 channel image compare to 3 channel image
			if (colorImg.type() == 24)
			{
				//cout << colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y)) << " - " << (cTemplate.at(r).at(c)) << "    -    " << Point(c + searhPoint.x, r + searhPoint.y) << " - " << Point(c, r) << " ";
				const int offset = abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[0] - (cTemplate.at(r).at(c)[0]))) +
					abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[1] - (cTemplate.at(r).at(c)[1]))) +
					abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[2] - (cTemplate.at(r).at(c)[2])));

				if (offset > 10)
				{
					cout << "No Fast Color Find - offset: " << offset << " image type: " << colorImg.type() << " ";
					return false;
				}
			}
			//3 channel image compare to 3 channel image
			else
			{
				//cout << colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y)) << " - " << (cTemplate.at(r).at(c)) << "    -    " << Point(c + searhPoint.x, r + searhPoint.y) << " - " << Point(c, r) << " ";
				const int offset = abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[0] - (cTemplate.at(r).at(c)[0]))) +
					abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[1] - (cTemplate.at(r).at(c)[1]))) +
					abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[2] - (cTemplate.at(r).at(c)[2])));
				if (offset != 0)
				{
					cout << "No Fast Color Find - offset: " << offset << " image type: " << colorImg.type() << " ";
					return false;
				}
			}
		}
	return true;
}

void ScreenInterpreter::imageTo2dCollorVec(Mat& colorImgInput, vector<vector<Vec3b>>& cVecOutput, Point size)
{
	for (int r = 0; r < size.y; r++)
	{
		vector<Vec3b> line;
		for (int c = 0; c < size.x; c++)
		{
			line.push_back(colorImgInput.at<Vec3b>(Point(c, r)));
		}
		cVecOutput.push_back(line);
	}
}

vector<vector<const MapElement*>> ScreenInterpreter::getGridPixels(ImageResource& img, Point2f binOffsets)
{
	const int boarder = 1;
	const int rezize = 2;

	vector<vector<const MapElement*>> map;
	map.reserve(expectedPoints.size());
	for (int r = boarder; r < expectedPoints.size() - boarder; r++)
	{
		vector<const MapElement*> mapLine;
		mapLine.reserve(expectedPoints.front().size());
		for (int c = boarder; c < expectedPoints.front().size() - boarder; c++)
		{
			vector<Vec4b> colors;
			colors.reserve((blockWidth - rezize*2) * (blockHeight - rezize * 2));

			for (int x = rezize; x < blockWidth - rezize; x++)
				for (int y = rezize; y < blockHeight - rezize; y++)
				{
					const int xpos = expectedPoints.at(r).at(c).x + x + binOffsets.x;
					const int ypos = expectedPoints.at(r).at(c).y + y + binOffsets.y;
					if (!((xpos < 0 || ypos < 0 || xpos >= img.getColor()->cols || ypos >= img.getColor()->rows)))
					{
						const Vec4b color = img.getColor()->at<Vec4b>(Point(xpos, ypos));
						if (!(color[0] == color[1] && color[1] == color[2]))
							colors.push_back(color);
					}
				}
			if (!colors.empty())
				mapLine.push_back(getMode(colors));
			else
				mapLine.push_back(mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255)));
		}
		map.push_back(mapLine);
	}
	return map;
}

const MapElement* ScreenInterpreter::getMode(vector<Vec4b> colors)
{
	Vec4b* number = &colors.front();
	Vec4b* mode = number;
	int count = 1;
	int countMode = 1;

	for (int i = 1; i < colors.size(); i++)
	{
		if (colors.at(i) == *number)
			++count;
		else
		{
			if (count > countMode)
			{
				countMode = count;
				mode = number;
			}
			count = 1;
			number = &colors.at(i);
		}
	}
	return mapElementCollection.searchMapElementByColor((Vec3b((*mode)[0], (*mode)[1], (*mode)[2])));
}