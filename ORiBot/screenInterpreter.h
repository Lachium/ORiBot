#pragma once
#include "imageResource.h"
#include "mapElement.h"

#define maxBinsX 47
#define maxBinsY 27
#define gameWindownHeight 720
#define gameWindownWidth 1280
#define blockWidth 6
#define blockHeight 6
#define shiftA 331
#define shiftB 140
#define shiftC 904
#define xOffsetConst 412
#define yOffsetConst 221
#define binWithShift 280
#define binWidth 26.667
#define binHeight 26.667


using namespace std;
using namespace cv;

class ScreenInterpreter
{
public:
	ScreenInterpreter();
	bool screenToMapElements(Mat& screenImg, vector<vector<MapElement*>>& world);
	Point getGlobalWindowOffset() { return gameLogoPos + Point(-6,18); };

private:
	Point2f getGridBinOffset(ImageResource& img);
	vector<vector<MapElement*>> calculateGridPixels(ImageResource& img, Point2f binOffsets);
	bool cropToGameWindow(ImageResource& img);
	bool singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, Point& matchPoint) const;
	bool colorSearchSingleMap(Mat& imgColor, Vec3b color, Point& matchPoint) const;
	bool singleColorMatchingFast(Mat& mInput, vector<vector<Vec3b>>& cTemplate, Point& matchPoint) const;
	MapElement* getMode(vector<Vec3b> &colors);
	vector<vector<Point2f>> expectedPoints;
	Point gameLogoPos = Point(-1, -1);
	ImageResourceCollection imageResourceCollection;
	MapElementCollection mapElementCollection;
};
