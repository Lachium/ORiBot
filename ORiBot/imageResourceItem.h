#pragma once
#include<opencv2/opencv.hpp>
#include<string>

using namespace std;
using namespace cv;

class imageResourceItem
{
public:
	Mat imgColor;
	Mat imGray;
	imageResourceItem(string imgPath) {
		imgColor = imread(imgPath);
		cvtColor(imgColor, imGray, COLOR_BGR2GRAY);
	};
	imageResourceItem(Mat img) {
		imgColor = img;
		cvtColor(imgColor, imGray, COLOR_BGR2GRAY);
	};
	imageResourceItem() {};

	bool cropImage(Rect boundry) {
		if (!(boundry.x < 0 || boundry.y < 0 || imgColor.rows < (boundry.height + boundry.y) || imgColor.cols < (boundry.width + boundry.x)))
		{
			imgColor = imgColor(boundry);
			imGray = imGray(boundry);
			return true;
		}
		else
		{
			cout << "Invalid Boundry " << boundry << "\n";
			return false;
		}
	};
};