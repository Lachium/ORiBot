#include<opencv2/opencv.hpp>
#include "ImageHandelingComponent.h"
#include "thought.h"


using namespace std;
using namespace cv;

ImageHandelingComponent imageHandelingComponent = ImageHandelingComponent();
Thought thought = Thought();

static void on_trackbar(int, void*)
{
	imageHandelingComponent.getGameGrid(imageHandelingComponent.expectedPoints);
}

int main(int argv, char** argc)
{
	clock_t start, end;
	//namedWindow("sliders", WINDOW_AUTOSIZE); // Create Window   

	//createTrackbar("ShiftA", "sliders", &imageHandelingComponent.shiftA, 4000, on_trackbar);
	//createTrackbar("ShiftB", "sliders", &imageHandelingComponent.shiftB, 200, on_trackbar);
	//createTrackbar("ShiftC", "sliders", &imageHandelingComponent.shiftC, 2000, on_trackbar);
	//createTrackbar("xOffsetConst", "sliders", &imageHandelingComponent.xOffsetConst, 5000, on_trackbar);
	//createTrackbar("yOffsetConst", "sliders", &imageHandelingComponent.yOffsetConst, 5000, on_trackbar);

	int key = 0;
	while (true)
	{
		start = clock();

		Mat world;
		if (imageHandelingComponent.camptureScreen(world))
		{
			//imshow("window", *imageHandelingComponent.imgScreen.getColor());
			thought.printMapConents(world);
		}
		end = clock();

		double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << fixed << time_taken * 1000 << setprecision(0); cout << "ms  "; cout << fixed << 1/time_taken << setprecision(1); cout << "FPS" << endl;


		//imshow("Gray Img", imgScreenGray);
		key = waitKey(10);
		//cout << "\x1B[2J\x1B[H";
	}
	waitKey(0);
	return 0;
}