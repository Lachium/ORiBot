#pragma once
#include<opencv2/opencv.hpp>
#include<string>

using namespace std;
using namespace cv;

class imageResourceItem
{
public:
	imageResourceItem(string imgPath);
	Mat imgColor;
	Mat imGray;
};

imageResourceItem::imageResourceItem(string imgPath)
{
	imgColor = imread(imgPath);
	cvtColor(imgColor, imGray, COLOR_BGR2GRAY);
}