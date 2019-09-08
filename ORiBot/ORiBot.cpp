#include<opencv2/opencv.hpp>
#include "ImageHandelingComponent.h"

using namespace std;
using namespace cv;

int main(int argv, char** argc)
{
	clock_t start, end;
	ImageHandelingComponent imageHandelingComponent = ImageHandelingComponent();

	int key = 0;
	while (true)
	{
		start = clock();
		imageHandelingComponent.camptureScreen();
		end = clock();

		if (imageHandelingComponent.cropToGameWindow())
		{
			imageHandelingComponent.drawGridBins();
		}
		else
			cout << "Could not Crop\n";

		double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
		cout << fixed << time_taken * 1000 << setprecision(0); cout << "ms  "; cout << fixed << 1/time_taken << setprecision(1); cout << "FPS" << endl;;
		imshow("Color Img", *imageHandelingComponent.imgScreen.getColor());
		//imshow("Gray Img", imgScreenGray);
		key = waitKey(10);
	}
	waitKey(0);
	return 0;
}