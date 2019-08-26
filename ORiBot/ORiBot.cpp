#include<opencv2/opencv.hpp>
#include<iostream>
#include <Windows.h>
#include <iostream>

#define w 400

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
	HWND hwndDesktop = GetDesktopWindow();
	namedWindow("output", WINDOW_NORMAL);
	Point pt = Point(10, 8);

	int key = 0;

	while (key != 27)
	{
		Mat img = hwnd2mat(hwndDesktop);
		img = img(Rect(0, 0, 500, 500));
		//Mat templ = img(Rect(100, 100, 128, 128));
		vector<Mat> splitChannels(3);
		split(img, splitChannels);
		/*
		for (int r = 0; r < img.rows; r++)
			for (int c = 0; c < img.cols; c++)
			{
				//img.at<Vec4b>(r, c)[0] = 75;
				//img.at<Vec4b>(r, c)[1] = 150;
				//img.at<Vec4b>(r, c)[2] = 255;
			}
			*/
		imshow("input", splitChannels[1]);
		imshow("output", img);

		// you can do some image processing here
		//imshow("output", MatchingMethod(img, templ));
		//imshow("template", templ);
		key = waitKey(1); // you can change wait time
	}
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
			img.at<Vec3b>(r, c)[0] = img.at<Vec3b>(r, c)[0] * 0 ;
			//rectangle(result, Point(i, j), Point(i + templateImg.cols, j + templateImg.rows), Scalar(0, 0, 255), 2, 8, 0);			
		}

	return img;
}