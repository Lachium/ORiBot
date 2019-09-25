#pragma once
#include <Windows.h>
//#include <iostream>
//#include <opencv2/opencv.hpp>
#include "ImageResources.h"

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
	ImageHandelingComponent();
	bool cropToGameWindow();
	void drawGridBins();
	Mat getGridPixels();
	bool camptureScreen(Mat & world);
	void getGameGrid(vector<vector<Point2f>>& outputVec);
	imageResourceItem imgScreen;

	double xOffset = 0.0;
	double yOffset = 0.0;
	int shiftA = 331;
	int shiftB = 140;
	int shiftC = 904;
	int xOffsetConst = 412;
	int yOffsetConst = 221;
	const double binWidth = 26.667;
	const double binHeight = 26.667;
	int binWithShift = 280;
	vector<vector<Point2f>> expectedPoints;

private:
	ImageResources imageResources;
	Point gameLogoPos = Point(-1,-1);
	Mat hwnd2mat(HWND hwnd);
	bool multipleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, vector<Point2f>& List_Matches);
	bool singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, Point& matchPoint);
	bool singleTemplateMatchingGreyExact(Mat& mInput, Mat& mTemplate, float Threshold, Point& startPoint);
	bool colorSearchSingle(Mat& imgColor, Vec3b color, Point& matchPoint);
	bool colorSearchSingleMap(Mat& imgColor, Vec3b color, Point& matchPoint);
	bool singleColorMatchingFast(Mat& mInput, vector<vector<Vec3b>>& cTemplate, Point& matchPoint);
	void imageTo2dCollorVec(Mat& colorImgInput, vector<vector<Vec3b>>& cVecOutput, Point size);
	Vec4b getMode(vector<Vec4b> colors);
};
