#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include "screenCapture.h"

using namespace std;
using namespace cv;

class imageResourceItem
{
public:
	imageResourceItem(string imgPath) {
		imgColor = imread(imgPath);
	};
	imageResourceItem(Mat img) {
		imgColor = img;
	};
	imageResourceItem() {};

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
			cout << "Invalid Boundry " << boundry << "\n";
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