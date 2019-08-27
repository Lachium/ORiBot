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

	//Mat mat = img;
	/*std::vector<float> array;

		array.assign((float*)mat.data, (float*)mat.data + mat.total());
		cout << "\n";
		cout << (float*)mat.data;
		cout << "\n";
		cout << (float*)mat.data + mat.total();
		cout << "\n";*/

	imshow("imageOri", img);
	namedWindow("imageOri", cv::WINDOW_AUTOSIZE);
	vector<Point> *gridPoints = new vector<Point>;

#define maxBinsX 42
#define maxBinsY 24
#define binWidth 25
#define binHeight 24
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
				//img.at<Vec3b>(Point(c, r)) = Vec3b::all('0');
			}
		}

	int xOffset = (int)(firstTile.x % binWidth);
	int yOffset = (int)(firstTile.y % binHeight);
	xOffset = 0;
	yOffset = 0;
	
	for (int r = 0; r < maxBinsY + 1; r++)
		for (int c = 0; c < maxBinsX +1; c++)
		{
			int startX = (int)(c * binWidth + xOffset);
			int startY = (int)(r * binHeight+ yOffset);
			Point start = Point(startX, startY);
			int endX = (int)(c * binWidth + blockWidth + xOffset);
			int endY = (int)(r * binHeight + blockHeight + yOffset);
			Point end = Point(endX, endY);

			if(startX < img.cols && endX < img.cols &&
				startX > 0 && endX > 0 && 
				startY < img.rows && endY < img.cols &&
				startY > 0 && endY > 0)
			cv::rectangle(img, start, end, cv::Scalar(0, 0, 255));
		}


	/*for (int r = 0; r < img.rows -1; r++)
	{
		vector<int> colstoSkipRowsAt;
		for (int c = 0; c < img.cols -1; c++)
		{
			Vec3b color = img.at<Vec3b>(Point(c, r));	
			if ((int)(color.val[0]) == 0 && (int)(color.val[1]) == 255 && (int)(color.val[2]) == 0)
			{
				gridPoints->push_back(Point(c, r));
				img.at<Vec3b>(Point(c, r)) = Vec3b::all('0');

				colstoSkipRowsAt.push_back(c);
				c += 20;
			}
		}
	}*/
	
	/*for (auto &iter : *gridPoints)
	{
		cout << iter << endl;
	}*/

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