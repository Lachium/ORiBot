#include <opencv2/opencv.hpp>
#include "screenInterpreter.h"
#include "mapStitcher.h"
#include "mapElement.h"
#include "InputEmulator.h"
#include <atomic>

using namespace std;
using namespace cv;

atomic<bool> shouldGameTerminate = false;

void ScreenCaptureThread(void);
thread screenCaptureThread(ScreenCaptureThread);
atomic<bool> shouldScreenCaptureTerminate = false;
HANDLE hEvent_ScreenCaptureThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_CAPTURE");

void StitchMapThread(void);
thread stictMapThread(StitchMapThread);
atomic<bool> shouldMappingTerminate = false;
HANDLE hEvent_StitchMapThread = CreateEvent(NULL, true, false, L"FIRE_STITCH_MAP");

atomic<bool> shouldRecapture = true;

Mat lastScreen;
vector<vector<MapElement*>> lastWorld;
vector<vector<MapElement*>> world;
deque<deque<MapTile>> gridMap;

ScreenCapture screenCapture;
ScreenInterpreter screenInterpreter = ScreenInterpreter();
MapStitcher mapStitcher = MapStitcher();
InputEmulator inputEmulator = InputEmulator();

void printTime(string name, clock_t start)
{
	double time_taken = double((clock() - start) / double(CLOCKS_PER_SEC));
	cout << "|" << name << " "<< fixed << time_taken * 1000 << setprecision(0); cout << "ms ";
	//cout << fixed << 1 / time_taken << setprecision(1); cout << "FPS|";
	cout << endl;
}

int main(int argv, char** argc)
{	
	while (!shouldGameTerminate)
	{
		clock_t start = clock();

		//Body--------###

		Mat* screen = screenCapture.readImage();
		while (!shouldRecapture);
		shouldRecapture = false;
		lastScreen = *screen;

		SetEvent(hEvent_ScreenCaptureThread);

		//Body--------###
		printTime("Capture" , start);
		waitKey(1);
	}
	shouldScreenCaptureTerminate = true;
	screenCaptureThread.join();
	waitKey(0);
	return 0;
}

void ScreenCaptureThread()
{
	//Thread Stuffs
	if (!hEvent_ScreenCaptureThread) assert(false);	
	//Thread Stuffs

	while (!shouldScreenCaptureTerminate)
	{
		WaitForSingleObject(hEvent_ScreenCaptureThread, INFINITE);

		clock_t start = clock();
		//Body--------###
		if (screenInterpreter.screenToMapElements(lastScreen, world))
		{

			lastWorld = world;
			SetEvent(hEvent_StitchMapThread);
			//inputEmulator.SetNumLock(true);

		}
		else
			shouldRecapture = true;
		//Body--------###

		printTime("World", start);
		ResetEvent(hEvent_ScreenCaptureThread);
		waitKey(1);
	}
	shouldMappingTerminate = true;
	stictMapThread.join();

	shouldGameTerminate = true;
}

void StitchMapThread()
{
	if (!hEvent_StitchMapThread) assert(false);

	while (!shouldMappingTerminate)
	{
		WaitForSingleObject(hEvent_StitchMapThread, INFINITE);
		//Body--------###

		clock_t start = clock();
		mapStitcher.appendToMap(lastWorld);
		printTime("Map", start); 
		shouldRecapture = true;
		ResetEvent(hEvent_StitchMapThread);

		//Body--------###
		waitKey(1);
	}
	shouldGameTerminate = true;
}
