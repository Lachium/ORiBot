#include<opencv2/opencv.hpp>
#include "ImageHandelingComponent.h"


using namespace std;
using namespace cv;

ImageHandelingComponent imageHandelingComponent = ImageHandelingComponent();

static void on_trackbar(int, void*)
{
	imageHandelingComponent.getGameGrid(imageHandelingComponent.expectedPoints);
}

int main(int argv, char** argc)
{
	clock_t start, end;
	namedWindow("sliders", WINDOW_AUTOSIZE); // Create Window   

	createTrackbar("ShiftA", "sliders", &imageHandelingComponent.shiftA, 1000, on_trackbar);
	createTrackbar("ShiftB", "sliders", &imageHandelingComponent.shiftB, 1000, on_trackbar);
	createTrackbar("ShiftC", "sliders", &imageHandelingComponent.shiftC, 1000, on_trackbar);
	createTrackbar("xOffsetConst", "sliders", &imageHandelingComponent.xOffsetConst, 1000, on_trackbar);
	createTrackbar("yOffsetConst", "sliders", &imageHandelingComponent.yOffsetConst, 1000, on_trackbar);

	int key = 0;
	while (true)
	{
		start = clock();
		imageHandelingComponent.camptureScreen();
		end = clock();

		if (imageHandelingComponent.cropToGameWindow())
		{
			imageHandelingComponent.drawGridBins();
			imageHandelingComponent.getGridPixels();
		}
		else
			cout << "Could not Crop\n";

		double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << fixed << time_taken * 1000 << setprecision(0); cout << "ms  "; cout << fixed << 1/time_taken << setprecision(1); cout << "FPS" << endl;


		imshow("window", *imageHandelingComponent.imgScreen.getColor());
		//imshow("Gray Img", imgScreenGray);
		key = waitKey(10);
	}
	waitKey(0);
	return 0;
}