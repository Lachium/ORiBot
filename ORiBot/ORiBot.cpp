#include <opencv2/opencv.hpp>
#include "screenInterpreter.h"
#include "mapStitcher.h"
#include "mapElement.h"
#include "InputEmulator.h"
#include <atomic>

using namespace std;
using namespace cv;

deque<deque<MapTile>> gridMap;

ScreenCapture screenCapture;
ScreenInterpreter screenInterpreter;
MapStitcher mapStitcher;
InputEmulator inputEmulator;

void printTime(string name, clock_t start, bool printFPS)
{

	double time_taken = double((clock() - start) / double(CLOCKS_PER_SEC));
	cout << "|" << name << " "<< fixed << time_taken * 1000 << setprecision(0); cout << "ms ";
	if(printFPS)
		cout << fixed << 1 / time_taken << setprecision(1); cout << "FPS|";

	cout << endl;
}

void StitchMapThread(vector<vector<MapElement*>> world)
{
	clock_t start = clock();
	mapStitcher.appendToMap(world);
	printTime("Map", start, false);
}

void WorldInterpreterThread(Mat screen)
{
	clock_t start = clock();
	vector<vector<MapElement*>> world;
	if (screenInterpreter.screenToMapElements(screen, world))
	{
		StitchMapThread(world);
	}
	printTime("World", start, false);
}

int main(int argv, char** argc)
{	
	while (true)
	{
		clock_t start = clock();

		Mat screen = screenCapture.readImage();

		WorldInterpreterThread(screen);
		printTime("Capture" , start, true);
		waitKey(1);
	}
	waitKey(0);
	return 0;
}

