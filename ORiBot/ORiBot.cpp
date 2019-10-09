#include<opencv2/opencv.hpp>
#include "screenInterpreter.h"
#include "mapStitcher.h"
#include "mapElement.h"


using namespace std;
using namespace cv;


int main(int argv, char** argc)
{
	ScreenInterpreter screenInterpreter = ScreenInterpreter();
	MapStitcher mapStitcher = MapStitcher();
	ScreenCapture screenCapture;
	vector<vector<const MapElement*>> world;
	
	int key = 0;
	while (true)
	{
		clock_t start = clock();

		clock_t start_Image_Capture = clock();
		Mat * screen = screenCapture.readImage();
		cout << "|Capt " << fixed << double((clock() - start_Image_Capture) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms|";

		clock_t start_World = clock();
		if (screenInterpreter.screenToMapElements(*screen, world))
		{
			cout << "|World " << fixed << double((clock() - start_World) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms|";
			clock_t start_Stitch = clock();
			mapStitcher.appendToMap(world);
			cout << "|Stitch " << fixed << double((clock() - start_Stitch) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms|";
		}
		double time_taken = double((clock() - start) / double(CLOCKS_PER_SEC));
		cout << "|Total " << fixed << time_taken * 1000 << setprecision(0); cout << "ms ";
		cout << fixed << 1 / time_taken << setprecision(1); cout << "FPS|";
		cout << endl;
		key = waitKey(1);
	}
	waitKey(0);
	return 0;
}