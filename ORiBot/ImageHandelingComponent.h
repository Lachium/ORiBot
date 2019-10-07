#pragma once
#include "ImageResources.h"
#include "mapElement.h"

#define maxBinsX 47
#define maxBinsY 27
#define gameWindownHeight 720
#define gameWindownWidth 1280
#define blockWidth 6
#define blockHeight 6

using namespace std;
using namespace cv;

class ImageHandelingComponent
{
public:
	MapElementCollection mapElementCollection;
	ImageHandelingComponent();
	Point2f getGridBinOffset(imageResourceItem & img);
	vector<vector<MapElement*>> getGridPixels(imageResourceItem & img, Point2f binOffsets);
	bool screenToMapElements(vector<vector<MapElement*>>& world);
	void getGameGrid(vector<vector<Point2f>>& outputVec);


private:
	int shiftA = 331;
	int shiftB = 140;
	int shiftC = 904;
	int xOffsetConst = 412;
	int yOffsetConst = 221;
	int binWithShift = 280;
	vector<vector<Point2f>> expectedPoints;
	const double binWidth = 26.667;
	const double binHeight = 26.667;
	ImageResources imageResources;
	Point gameLogoPos = Point(-1,-1);
	bool cropToGameWindow(imageResourceItem& img);
	bool multipleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, vector<Point2f>& List_Matches);
	bool singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, Point& matchPoint);
	bool singleTemplateMatchingGreyExact(Mat& mInput, Mat& mTemplate, float Threshold, Point& startPoint);
	bool colorSearchSingle(Mat& imgColor, Vec3b color, Point& matchPoint);
	bool colorSearchSingleMap(Mat& imgColor, Vec3b color, Point& matchPoint);
	bool singleColorMatchingFast(Mat& mInput, vector<vector<Vec3b>>& cTemplate, Point& matchPoint);
	void imageTo2dCollorVec(Mat& colorImgInput, vector<vector<Vec3b>>& cVecOutput, Point size);
	MapElement * getMode(vector<Vec4b> colors);
};
