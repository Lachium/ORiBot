#include<opencv2/opencv.hpp>
#include<iostream>
#include <Windows.h>
#include <iostream>

using namespace std;
using namespace cv;

Mat MatchingMethod(Mat img, Mat templateImg);

Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	src.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)& bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main(int argv, char** argc)
{
	/*HWND hwndDesktop = GetDesktopWindow();
	int key = 0;
	while (key != 27)
	{
		Mat img = hwnd2mat(hwndDesktop);

		key = waitKey(1);
	}*/

	Mat img = imread("../Content/img/capture.bmp");
	//img = img(Rect(0, 0, 128, 64));


	//imshow("imageOri", img);
	//namedWindow("imageOri", cv::WINDOW_AUTOSIZE);

#define maxBinsX 49
#define maxBinsY 27
#define binWidth (1280/48)
#define binHeight (720/27)
#define blockWidth 6
#define blockHeight 6

	//EstimateBins	
	Point firstTile;
	for (int r = 0; r < img.rows -1; r++)
		for (int c = 0; c < img.cols - 1; c++)
		{
			Vec3b color = img.at<Vec3b>(Point(c, r));
			if ((int)(color.val[0]) == 0 && (int)(color.val[1]) == 255 && (int)(color.val[2]) == 0)
			{
				firstTile = Point(c, r);
				r = img.rows;
				break;
			}
		}
	
	Point2f estimatedBin = Point2f(firstTile.x/binWidth, firstTile.y / binHeight);

	
	int xOffset = 14;
	int yOffset = 10;
	vector<Point>* expectedPoints = new vector<Point>;
	for (int r = 0; r < maxBinsY +1; r++)
		for (int c = 0; c < maxBinsX +1; c++)
		{
			double xShift = ((-(maxBinsX * 0.5) + c) * 0.8) + (-(maxBinsX * 0.5) + c) * ((-(maxBinsY * 0.5) + r)) * 0.014;
			double yShift = r * 0.82;

			int startX = (int)(c * binWidth + xOffset + xShift);
			int startY = (int)(r * binHeight+ yOffset + yShift);
			Point start = Point(startX, startY);
			int endX = (int)(start.x + blockWidth);
			int endY = (int)(start.y + blockHeight);
			Point end = Point(endX, endY);

			expectedPoints->push_back(Point(startX, startY));
		}

	int expectedBin = estimatedBin.y * (maxBinsX + 1) + estimatedBin.x;
	xOffset = (int)(expectedPoints->at(expectedBin).x - firstTile.x);
	yOffset = (int)(expectedPoints->at(expectedBin).y - firstTile.y);
	Point start = expectedPoints->at(expectedBin) + Point(xOffset, yOffset);
	int endX = (int)(start.x + blockWidth);
	int endY = (int)(start.y + blockHeight);
	Point end = Point(endX, endY);

	for (auto &iter : *expectedPoints)
	{
		cv::rectangle(img, iter, Point(iter.x + blockWidth, iter.y + blockHeight), cv::Scalar(255, 100, 0));
	}

	//namedWindow("image", WINDOW_NORMAL);
	imshow("image", img);
	namedWindow("image", cv::WINDOW_AUTOSIZE);
	waitKey(0);
	//}
}

Mat MatchingMethod(Mat img, Mat templateImg)
{
	Mat img_display;
	img.copyTo(img_display);
	int result_cols = img.cols - templateImg.cols + 1;
	int result_rows = img.rows - templateImg.rows + 1;

	Mat result;

	matchTemplate(img, templateImg, result, TM_SQDIFF_NORMED);
	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());


	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	matchLoc = minLoc;
	list<Point> thresh;
	int threshold = 1;
	for (int r = 0; r < img.cols; r++)
		for (int c = 0; c < img.rows; c++)
			//if(result.at<uchar>(Point(i, j)) < threshold)
		{
			img.at<Vec3b>(r, c)[0] = img.at<Vec3b>(r, c)[0] * 0;
			//rectangle(result, Point(i, j), Point(i + templateImg.cols, j + templateImg.rows), Scalar(0, 0, 255), 2, 8, 0);            
		}

	return img;
}