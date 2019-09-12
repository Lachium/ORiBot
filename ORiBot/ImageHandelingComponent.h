#pragma once
#include <Windows.h>
//#include <iostream>
//#include <opencv2/opencv.hpp>
#include "ImageResources.h"

#define maxBinsX 49
#define maxBinsY 27
#define gameWindownHeight 720
#define gameWindownWidth 1280
#define binWidth (gameWindownWidth/(maxBinsX-1))
#define binHeight (gameWindownHeight/maxBinsY)
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
	void getGridPixels();
	void camptureScreen();
	void getGameGrid(vector<vector<Point>>& outputVec);
	imageResourceItem imgScreen;
	int xOffset = 0;
	int yOffset = 0;
	int shiftA = 828;
	int shiftB = 14;
	int shiftC = 718;
	int xOffsetConst = 710;
	int yOffsetConst = 615;
	vector<vector<Point>> expectedPoints;

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
