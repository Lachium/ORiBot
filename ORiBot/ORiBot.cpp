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
atomic<int> globalOffsetX, globalOffsetY;

void ScreenInterpreterThread(void);
atomic<bool> shouldScreenInterpreterTerminate = false;
HANDLE hEvent_ScreenInterpreterThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_Interpreter");

void StitchMapThread(void);
atomic<bool> shouldMappingTerminate = false;
HANDLE hEvent_StitchMapThread = CreateEvent(NULL, true, false, L"FIRE_STITCH_MAP");

HANDLE hEvent_ScreenCaptureThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_CAPTURE");

Mat screen;
vector<vector<MapElement*>> world;
deque<deque<MapTile>> gridMap; 
vector<Point> route;

ScreenCapture screenCapture;
ScreenInterpreter screenInterpreter = ScreenInterpreter();
MapStitcher mapStitcher = MapStitcher();
InputEmulator inputEmulator = InputEmulator();
Navigator navigator = Navigator();

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
		ORiUtils::ConsoleLogTimed("Capture" , start);
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
			Point globalOffset = screenInterpreter.getGlobalWindowOffset();
			globalOffsetX = globalOffset.x;
			globalOffsetY = globalOffset.y;

			SetEvent(hEvent_StitchMapThread);
		}
		else
			SetEvent(hEvent_ScreenCaptureThread);
		//Body--------###

		ORiUtils::ConsoleLogTimed("World", start);
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
			if (mapStitcher.didGridGrow() || route.size() ==0)
			{
				route = navigator.doPathFinding(mapStitcher.getGridMap(), mapStitcher.getMyGridPos());
			}
			else if(route.size() > 0)
			{
				inputEmulator.followRoute(route, mapStitcher.getMyGridPos());
			}


			inputEmulator.setGlobalOffset(globalOffsetX, globalOffsetY);
			//inputEmulator.moveCursorAndClick(0,0);
			
			/*const deque<deque<MapTile>>& getGridMap() { return gridMap; };
			Point getMyGridPos() { return myGridPos; };*/
		}
		
		
		ORiUtils::ConsoleLogTimed("Map", start);
		ResetEvent(hEvent_StitchMapThread);

		//Body--------###
		waitKey(1);
	}
	shouldGameTerminate = true;
}
