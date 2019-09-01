#include "ImageHandelingComponent.h"

ImageHandelingComponent::ImageHandelingComponent()
{
	imageResources = ImageResources();
}

void ImageHandelingComponent::camptureScreen()
{
	imgScreen = imageResourceItem(hwnd2mat(GetDesktopWindow()));
}

Mat ImageHandelingComponent::hwnd2mat(HWND hwnd) {

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
	height = windowsize.bottom;  //change this to whatever size you want to resize to
	width = windowsize.right;

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
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	return src;
}

bool ImageHandelingComponent::multipleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float mthreshold, float closeness, vector<Point2f>& listmatches)
{
	Mat mResult;
	Size mTemplateSz = mTemplate.size();
	Size mtemplateCloseRadiusSz((mTemplateSz.width / 2) * closeness, (mTemplateSz.height / 2) * closeness);

	matchTemplate(mInput, mTemplate, mResult, TM_CCOEFF_NORMED);
	threshold(mResult, mResult, mthreshold, 1.0, THRESH_TOZERO);
	while (true)
	{
		double minval, maxval;
		Point minloc, maxloc;
		minMaxLoc(mResult, &minval, &maxval, &minloc, &maxloc);

		if (maxval >= mthreshold)
		{
			listmatches.push_back(maxloc);
			rectangle(mResult, Point2f(maxloc.x - mtemplateCloseRadiusSz.width, maxloc.y - mtemplateCloseRadiusSz.height), Point2f(maxloc.x + mtemplateCloseRadiusSz.width, maxloc.y + mtemplateCloseRadiusSz.height), Scalar(0), -1);
		}
		else
			break;
	}
	return true;
}

bool ImageHandelingComponent::singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, float Closeness, Point& matchPoint)
{
	Mat mResult;
	Size szTemplate = mTemplate.size();
	Size szTemplateCloseRadius((szTemplate.width / 2) * Closeness, (szTemplate.height / 2) * Closeness);

	matchTemplate(mInput, mTemplate, mResult, TM_CCOEFF_NORMED);
	threshold(mResult, mResult, Threshold, 1.0, THRESH_TOZERO);

	double minval, maxval;
	Point minloc, maxloc;
	minMaxLoc(mResult, &minval, &maxval, &minloc, &maxloc);

	if (maxval >= Threshold)
	{
		matchPoint = (maxloc);
		return true;
	}
	else
	{
		cout << "No Match Found\n";
		return false;
	}
}

bool ImageHandelingComponent::singleTemplateMatchingGreyExact(Mat& mInput, Mat& mTemplate, float Threshold, Point& startPoint)
{
	Rect boundry(startPoint.x, startPoint.y, mTemplate.cols, mTemplate.rows);
	if (boundry.x < 0 || boundry.y < 0)
	{
		cout << "Invalid ExactPos: " << startPoint << "\n";
		return false;
	}
	Mat imgCropped = mInput(boundry);

	Mat mResult;
	matchTemplate(imgCropped, mTemplate, mResult, TM_CCOEFF_NORMED);
	threshold(mResult, mResult, Threshold, 1.0, THRESH_TOZERO);

	double minval, maxval;
	Point minloc, maxloc;
	minMaxLoc(mResult, &minval, &maxval, &minloc, &maxloc);

	if (maxval >= Threshold)
	{
		cout << "Fast Matched\n";
		return true;
	}
	else
	{
		cout << "No Exact Match Found\n";
		return false;
	}
}

vector<Point>* ImageHandelingComponent::getGameGrid()
{
	vector<Point>* expectedPoints = new vector<Point>;

	for (int r = 0; r < maxBinsY + 1; r++)
		for (int c = 0; c < maxBinsX + 1; c++)
		{
			double xShift = ((-(maxBinsX * 0.5) + c) * 0.8) + (-(maxBinsX * 0.5) + c) * ((-(maxBinsY * 0.5) + r)) * 0.014;
			double yShift = r * 0.82;

			int startX = (int)(c * binWidth + 14 + xShift);
			int startY = (int)(r * binHeight + 10 + yShift);
			Point start = Point(startX, startY);
			int endX = (int)(start.x + blockWidth);
			int endY = (int)(start.y + blockHeight);
			Point end = Point(endX, endY);

			expectedPoints->push_back(Point(startX, startY));
		}
	return expectedPoints;
}

Point ImageHandelingComponent::colorSearchSingle(Mat& imgColor, Vec3b color)
{
	for (int c = 0; c < imgColor.cols- 1; c++)
		for (int r = 0; r < imgColor.rows - 1; r++)
			if (imgColor.at<Vec3b>(Point(c, r)) == color)
				return Point(c, r);

	return Point(-1, -1);
}

void ImageHandelingComponent::drawGridBins()
{
	Point firstTile = colorSearchSingle(imgScreen.imgColor, Vec3b(0, 255, 0));
	Point2f estimatedBin = Point2f(firstTile.x / binWidth, firstTile.y / binHeight);
	vector<Point>* expectedPoints = getGameGrid();

	int expectedBin = estimatedBin.y * (maxBinsX + 1) + estimatedBin.x;
	int xOffset = (int)(expectedPoints->at(expectedBin).x - firstTile.x);
	int yOffset = (int)(expectedPoints->at(expectedBin).y - firstTile.y);
	Point start = expectedPoints->at(expectedBin) + Point(xOffset, yOffset);
	int endX = (int)(start.x + blockWidth);
	int endY = (int)(start.y + blockHeight);
	Point end = Point(endX, endY);

	for (Point iter : *expectedPoints)
	{
		rectangle(imgScreen.imgColor, iter, Point(iter.x + blockWidth, iter.y + blockHeight), cv::Scalar(255, 100, 0));
	}
}

bool ImageHandelingComponent::cropToGameWindow()
{
	if (singleTemplateMatchingGreyExact(imgScreen.imGray, imageResources.imgGameLogo.imGray, 0.9, gameLogoPos))
		return imgScreen.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));
	else {
		if (singleTemplateMatchingGrey(imgScreen.imGray, imageResources.imgGameLogo.imGray, 0.9, 0.9, gameLogoPos))
		{
			imgScreen.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));
			return true;
		}
		else
			return false;
	}
}