#include <opencv2/opencv.hpp>
#include "screenInterpreter.h"
#include "mapStitcher.h"
#include "mapElement.h"
#include "InputEmulator.h"
#include "navigation.h"
#include <atomic>

using namespace std;
using namespace cv;

atomic<bool> shouldGameTerminate = false;

void ScreenInterpreterThread(void);
atomic<bool> shouldScreenInterpreterTerminate = false;
HANDLE hEvent_ScreenInterpreterThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_Interpreter");

void StitchMapThread(void);
atomic<bool> shouldMappingTerminate = false;
HANDLE hEvent_StitchMapThread = CreateEvent(NULL, true, false, L"FIRE_STITCH_MAP");

HANDLE hEvent_ScreenCaptureThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_CAPTURE");

mutex mtx;

Mat screen;
vector<vector<MapElement*>> world;
deque<deque<MapTile>> gridMap;

ScreenCapture screenCapture;
ScreenInterpreter screenInterpreter = ScreenInterpreter();
MapStitcher mapStitcher = MapStitcher();
InputEmulator inputEmulator = InputEmulator();
Navigator navigator = Navigator();

void printTime(string name, clock_t start)
{
	mtx.lock();
	double time_taken = double((clock() - start) / double(CLOCKS_PER_SEC));
	cout << "|" << name << " "<< fixed << time_taken * 1000 << setprecision(0); cout << "ms ";
	cout << fixed << 1 / time_taken << setprecision(1); cout << "FPS|";
	cout << endl;
	mtx.unlock();
}

int main(int argv, char** argc)
{
	thread screenInterpreterThread(ScreenInterpreterThread);

	while (!shouldGameTerminate)
	{
		clock_t start = clock();

		//Body--------###
		screen = screenCapture.readImage();
		SetEvent(hEvent_ScreenInterpreterThread);

		//Body--------###
		//printTime("Capture" , start);
		waitKey(1);
		WaitForSingleObject(hEvent_ScreenCaptureThread, INFINITE);
	}
	shouldScreenInterpreterTerminate = true;
	screenInterpreterThread.join();
	waitKey(0);
	return 0;
}

void ScreenInterpreterThread()
{
	thread stictMapThread(StitchMapThread);
	//Thread Stuffs
	if (!hEvent_ScreenInterpreterThread) assert(false);
	//Thread Stuffs

	while (!shouldScreenInterpreterTerminate)
	{
		WaitForSingleObject(hEvent_ScreenInterpreterThread, INFINITE);
		Mat lastScreen = screen;

		clock_t start = clock();
		//Body--------###
		if (screenInterpreter.screenToMapElements(lastScreen, world))
		{
			SetEvent(hEvent_StitchMapThread);
			//inputEmulator.SetNumLock(true);
		}
		else
			SetEvent(hEvent_ScreenCaptureThread);
		//Body--------###

		//printTime("World", start);
		ResetEvent(hEvent_ScreenInterpreterThread);
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
		vector<vector<MapElement*>> lastWorld = world;
		SetEvent(hEvent_ScreenCaptureThread);
		//Body--------###

		clock_t start = clock();
		if (mapStitcher.appendToMap(lastWorld))
		{
			if (mapStitcher.didGridGrow())
				navigator.doPathFinding(mapStitcher.getGridMap(), mapStitcher.getMyGridPos());

			/*const deque<deque<MapTile>>& getGridMap() { return gridMap; };
			Point getMyGridPos() { return myGridPos; };*/
		}
		
		
		//printTime("Map", start); 
		ResetEvent(hEvent_StitchMapThread);

		//Body--------###
		waitKey(1);
	}
	shouldGameTerminate = true;
}
