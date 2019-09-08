#include "ImageHandelingComponent.h"

ImageHandelingComponent::ImageHandelingComponent()
{
	//imageResources = ImageResources();
	getGameGrid(expectedPoints);
}

void ImageHandelingComponent::camptureScreen()
{
	//Mat savedCapt = imread("../Content/img/screen.bmp"); // Fix mat.type
	//imgScreen = imageResourceItem("../Content/img/screen.bmp"); // Fix mat.type
	//Mat scCap = (hwnd2mat(GetDesktopWindow()));
	//cv::cvtColor(scCap, scCap, COLOR_BGRA2BGR);
	//cout << scCap.type();
	//imgScreen = imageResourceItem(hwnd2mat(GetDesktopWindow())); // Fix mat.type
	//imgScreen = imageResourceItem("../Content/img/screen.bmp"); // Fix mat.type	
	//imgScreen = imageResourceItem("../Content/img/screen.bmp"); // Fix mat.type

	imgScreen = imageResourceItem("../Content/img/screenshifted.bmp"); // Fix mat.type
	//imgScreen = imageResourceItem(hwnd2mat(GetDesktopWindow())); // Fix mat.type
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

bool ImageHandelingComponent::multipleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float mthreshold, vector<Point2f>& listmatches)
{
	Mat mResult;
	matchTemplate(mInput, mTemplate, mResult, TM_CCOEFF_NORMED);
	threshold(mResult, mResult, mthreshold, 1.0, THRESH_TOZERO);
	while (true)
	{
		double minval, maxval;
		Point minloc, maxloc;
		minMaxLoc(mResult, &minval, &maxval, &minloc, &maxloc);

		if (maxval >= mthreshold)
			listmatches.push_back(maxloc);
		else
			break;
	}
	return true;
}

bool ImageHandelingComponent::singleTemplateMatchingGrey(Mat& mInput, Mat& mTemplate, float Threshold, Point& matchPoint)
{
	Mat mResult;
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

void ImageHandelingComponent::getGameGrid(vector<Point>& outputVec)
{
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

			outputVec.push_back(Point(startX, startY));
		}
}

bool ImageHandelingComponent::colorSearchSingle(Mat& colorImg, Vec3b color, Point & matchPoint)
{
	int offset;
	for (int r = 0; r < colorImg.rows -1; r++)
		for (int c = 0; c < colorImg.cols - 1; c++)
		{
			//4 channel image compare to 3 channel image
			if (colorImg.type() == 24)
			{
				//cout << colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y)) << " - " << (cTemplate.at(r).at(c)) << "    -    " << Point(c + searhPoint.x, r + searhPoint.y) << " - " << Point(c, r) << endl;
				offset = abs((colorImg.at<Vec4b>(Point(c, r))[0] - color[0])) +
					abs((colorImg.at<Vec4b>(Point(c, r))[1] - color[1])) +
					abs((colorImg.at<Vec4b>(Point(c, r))[2] - color[2]));

				if (offset < 1)
				{
					cout << "offset: " << offset << endl;
					matchPoint = Point(c, r);
					return true;
				}
			}
			//3 channel image compare to 3 channel image
			else
			{
				//cout << colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y)) << " - " << (cTemplate.at(r).at(c)) << "    -    " << Point(c + searhPoint.x, r + searhPoint.y) << " - " << Point(c, r) << endl;
				offset = abs((colorImg.at<Vec3b>(Point(c, r))[0] - color[0])) +
					abs((colorImg.at<Vec3b>(Point(c, r))[1] - color[1])) +
					abs((colorImg.at<Vec3b>(Point(c, r))[2] - color[2]));

				if (offset == 0)
				{
					matchPoint = Point(c, r);
					return true;
				}
			}
		}
	cout << "No Fast Color Find - offset: " << offset << " image type: " << colorImg.type() << endl;
	return false;
}

void ImageHandelingComponent::drawGridBins()
{
	Point firstTile;
	if (!colorSearchSingle(*imgScreen.getColor(), Vec3b(0, 255, 0), firstTile))
		return;

	Point2f estimatedBin = Point2f(firstTile.x / binWidth, firstTile.y / binHeight);

	int expectedBin = estimatedBin.y * (maxBinsX + 1) + estimatedBin.x;
	xOffset = -(int)(expectedPoints.at(expectedBin).x - firstTile.x);
	yOffset = -2-(int)(expectedPoints.at(expectedBin).y - firstTile.y);
	Point start = expectedPoints.at(expectedBin) + Point(xOffset, yOffset);
	int endX = (int)(start.x + blockWidth);
	int endY = (int)(start.y + blockHeight);
	Point end = Point(endX, endY);


	for (Point iter : expectedPoints)
	{
		//rectangle(*imgScreen.getColor(), Point(iter.x + xOffset, iter.y + yOffset), Point(iter.x + blockWidth + xOffset, iter.y + blockHeight + yOffset), cv::Scalar(255, 100, 0));
	}
	rectangle(*imgScreen.getColor(), start, end, cv::Scalar(255, 100, 255), 5);
}

bool ImageHandelingComponent::cropToGameWindow()
{

	vector<vector<Vec3b>> cVecOutput;
	imageTo2dCollorVec(*imageResources.imgGameLogo.getColor(), cVecOutput, Point(2, 2));

	if (singleColorMatchingFast(*imgScreen.getColor(), cVecOutput, gameLogoPos))
		return imgScreen.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));
	else {
		if (singleTemplateMatchingGrey(*imgScreen.getGray(), *imageResources.imgGameLogo.getGray(), 0.9, gameLogoPos))
		{
			;//return imgScreen.cropImage(Rect(gameLogoPos.x - 6, gameLogoPos.y + 18, gameWindownWidth, gameWindownHeight));
		}
		else
			return false;
	}

	return false;
}

bool ImageHandelingComponent::singleColorMatchingFast(Mat& colorImg, vector<vector<Vec3b>>& cTemplate, Point& searhPoint)
{
	if (((cTemplate.at(0).size() + searhPoint.x) >= colorImg.cols) || ((cTemplate.size() + searhPoint.y) >= colorImg.rows) || searhPoint.x < 0 || searhPoint.y < 0)
	{
		cout << "Fast color search point out of bounds" << endl;
		return false;
	}
	for (int r = 0; r < cTemplate.size(); r++)
		for (int c = 0; c < cTemplate.at(0).size(); c++)
		{
			int offset;
			//4 channel image compare to 3 channel image
			if (colorImg.type() == 24)
			{
				//cout << colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y)) << " - " << (cTemplate.at(r).at(c)) << "    -    " << Point(c + searhPoint.x, r + searhPoint.y) << " - " << Point(c, r) << endl;
				offset = abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[0] - (cTemplate.at(r).at(c)[0]))) +
					abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[1] - (cTemplate.at(r).at(c)[1]))) +
					abs((colorImg.at<Vec4b>(Point(c + searhPoint.x, r + searhPoint.y))[2] - (cTemplate.at(r).at(c)[2])));

				if (offset > 10)
				{
					cout << "No Fast Color Find - offset: " << offset << " image type: " << colorImg.type() << endl;
					return false;
				}
			}
			//3 channel image compare to 3 channel image
			else
			{
				//cout << colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y)) << " - " << (cTemplate.at(r).at(c)) << "    -    " << Point(c + searhPoint.x, r + searhPoint.y) << " - " << Point(c, r) << endl;
				offset = abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[0] - (cTemplate.at(r).at(c)[0]))) +
					abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[1] - (cTemplate.at(r).at(c)[1]))) +
					abs((colorImg.at<Vec3b>(Point(c + searhPoint.x, r + searhPoint.y))[2] - (cTemplate.at(r).at(c)[2])));
				if (offset != 0)
				{
					cout << "No Fast Color Find - offset: " << offset << " image type: " << colorImg.type() << endl;
					return false;
				}
			}
		}
	return true;
}

void ImageHandelingComponent::imageTo2dCollorVec(Mat& colorImgInput, vector<vector<Vec3b>>& cVecOutput, Point size)
{
	for (int r = 0; r < size.y; r++)
	{
		vector<Vec3b> line;
		for (int c = 0; c < size.x; c++)
		{
			line.push_back(colorImgInput.at<Vec3b>(Point(c, r)));
		}
		cVecOutput.push_back(line);
	}
}

void ImageHandelingComponent::getGridPixels()
{
	Mat look = Mat::zeros((int)maxBinsY * blockHeight, (int)maxBinsX * blockWidth, CV_8UC3);

	for (int i = 0; i < expectedPoints.size(); i++)
	{
		for (int x = blockWidth / 2; x < blockWidth; x++)
			for (int y = blockHeight / 2; y < blockHeight; y++)
			{
				int xpos = expectedPoints.at(i).x + x + xOffset;
				int ypos = expectedPoints.at(i).y + y + yOffset;
				if (!(xpos< 0 || ypos < 0 || xpos > imgScreen.getColor()->cols - 1 || ypos > imgScreen.getColor()->rows - 1))
				{
					//cout << iter.x << " , " << iter.y << ") = (" << xpos << "," << ypos << endl;
					imgScreen.getColor()->at<Vec3b>(Point(xpos, ypos)) = Vec3b::all(210);
					look.at<Vec3b>(Point(x, y)) = imgScreen.getColor()->at<Vec3b>(Point(xpos, ypos));
				}
			}
	}
	imshow("look", look);
}