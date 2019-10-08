#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include "screenCapture.h"

#define dir "C:/ORiBot/Content/img/"

using namespace std;
using namespace cv;

class ImageResource
{
public:
	ImageResource(string imgPath) {
		imgColor = imread(imgPath);
	};
	ImageResource(Mat img) {
		imgColor = img;
	};

	bool cropImage(Rect boundry) {
		if (!(boundry.x < 0 || boundry.y < 0 || imgColor.rows < (boundry.height + boundry.y) || imgColor.cols < (boundry.width + boundry.x)))
		{
			imgColor = imgColor(boundry);
			if (!imGray.empty())
				imGray = imGray(boundry);
			return true;
		}
		else
		{
			cout << "Invalid Boundry " << boundry;
			return false;
		}
	};

	Mat* getGray() {
		if (imGray.empty())
			cvtColor(imgColor, imGray, COLOR_BGR2GRAY);
		return &imGray;
	};

	Mat* getColor() {
		return &imgColor;
	};

private:
	Mat imGray;
	Mat imgColor;
};

class ImageResourceCollection
{
public:
	const String directory = dir;
	ImageResource imgGameLogo = ImageResource(directory + "templates/gameLogo.bmp");
};