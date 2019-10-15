#pragma once

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <iostream>

using namespace std;
using namespace cv;

class ScreenCapture
{
public:
	ScreenCapture() {};
	Mat img;

	Mat * readImage()
	{
		img = Mat(hwnd2mat());
		return &img;
	};

private:
	const Mat hwnd2mat(HWND hwnd = GetDesktopWindow()) {

		const HDC hwindowDC = GetDC(hwnd);
		const HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
		SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

		RECT windowsize;    // get the height and width of the screen
		GetClientRect(hwnd, &windowsize);

		const int srcheight = windowsize.bottom;
		const int srcwidth = windowsize.right;
		const int height = windowsize.bottom;  //change this to whatever size you want to resize to
		const int width = windowsize.right;

		Mat src(height, width, CV_8UC4);

		// create a bitmap
		const HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
		BITMAPINFOHEADER  bi;
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
		DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

		return src;
	};
};