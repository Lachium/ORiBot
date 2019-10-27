#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include "screenCapture.h"
#include "oRiUtils.h"

#define Imgdir "C:/ORiBot/Content/img/"

using namespace std;
using namespace cv;

class ImageResource
{
public:
	ImageResource(const string imgPath) {
		imgColor = imread(imgPath, IMREAD_COLOR);
	};
	ImageResource(const Mat img) {
		imgColor = img;
	};

	bool cropImage(const Rect boundry) {
		if (!(boundry.x < 0 || boundry.y < 0 || imgColor.rows < (boundry.height + boundry.y) || imgColor.cols < (boundry.width + boundry.x)))
		{
			imgColor = imgColor(boundry);
			if (!imGray.empty())
				imGray = imGray(boundry);
			return true;
		}
		else
		{
			ORiUtils::ConsoleLog("Invalid Boundry ");
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

	vector<vector<Vec3b>>* getVec2D() {
		if (!vec2D_created)
		{
			imageToVec2D();
			vec2D_created = true;
		}
		return &imgColorVec2D;
	};

	void imageToVec2D()
	{
		const int rowSize = (maxVecSize.y < (int)imgColor.rows) ? maxVecSize.y : (int)imgColor.rows;
		const int colSize = (maxVecSize.x < (int)imgColor.cols) ? maxVecSize.x : (int)imgColor.cols;
		imgColorVec2D.reserve((int)imgColor.rows);
		for (int r = 0; r < rowSize; r++)
		{
			vector<Vec3b> line;
			line.reserve((int)imgColor.cols);
			for (int c = 0; c < colSize; c++)
			{
				line.push_back(imgColor.at<Vec3b>(Point(c, r)));
			}
			imgColorVec2D.push_back(line);
		}
	};

private:
	Mat imGray;
	Mat imgColor; vector<vector<Vec3b>> imgColorVec2D;
	bool vec2D_created = false;
	Point maxVecSize = Point(2, 2);
};

class ImageResourceCollection
{
public:
	const String directory = Imgdir;
	ImageResource imgGameLogo = ImageResource(directory + "templates/gameLogo.bmp");
};