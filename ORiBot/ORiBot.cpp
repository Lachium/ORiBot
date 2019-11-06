#include <opencv2/opencv.hpp>
#include "screenInterpreter.h"
#include "mapStitcher.h"
#include "mapElement.h"
#include "InputEmulator.h"
#include "navigation.h"
#include "environment.h"
#include <future>
#include <atomic>

using namespace std;
using namespace cv;

atomic<bool> shouldGameTerminate = false;
atomic<int> globalOffsetX, globalOffsetY;

void reactToWorld(vector<vector<MapElement*>>& lastWorld, Point2f lastInternalCellOffset);

void ScreenInterpreterThread(void);
atomic<bool> shouldScreenInterpreterTerminate = false;
HANDLE hEvent_ScreenInterpreterThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_Interpreter");

void StitchMapThread(void);
atomic<bool> shouldMappingTerminate = false;
HANDLE hEvent_StitchMapThread = CreateEvent(NULL, true, false, L"FIRE_STITCH_MAP");

HANDLE hEvent_ScreenCaptureThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_CAPTURE");

atomic<bool> pathfindingDone = true;
vector<Point> FindPath(deque<deque<MapTile>> gridMap, Point DolPoss);


Mat screen;
vector<vector<MapElement*>> world;
Point2f internalCellOffset;
//deque<deque<MapTile>> gridMap; 
vector<Point> route;

ScreenCapture screenCapture;
ScreenInterpreter screenInterpreter = ScreenInterpreter();
MapStitcher mapStitcher = MapStitcher();
static InputEmulator inputEmulator = InputEmulator();
Navigator navigator = Navigator();


int main(int argv, char** argc)
{
	//thread screenInterpreterThread(ScreenInterpreterThread);
	//thread stictMapThread(StitchMapThread);

	while (!shouldGameTerminate)
	{
		clock_t start = clock();

		//Body--------###
		screen = screenCapture.readImage();
		if (screenInterpreter.screenToMapElements(screen, world, internalCellOffset))
		{
			Point globalOffset = screenInterpreter.getGlobalWindowOffset();
			globalOffsetX = globalOffset.x;
			globalOffsetY = globalOffset.y;

			reactToWorld(world, internalCellOffset);
		}

		//Body--------###
		//ORiUtils::ConsoleLogTimed("Capture" , start);
		waitKey(1);
		//WaitForSingleObject(hEvent_ScreenCaptureThread, INFINITE);
	}
	shouldScreenInterpreterTerminate = true;
	//screenInterpreterThread.join();
	//stictMapThread.join();
	waitKey(0);
	return 0;
}

void ScreenInterpreterThread()
{
	//Thread Stuffs
	if (!hEvent_ScreenInterpreterThread) assert(false);
	//Thread Stuffs

	while (!shouldScreenInterpreterTerminate)
	{
		WaitForSingleObject(hEvent_ScreenInterpreterThread, INFINITE);
		Mat lastScreen = screen;

		clock_t start = clock();
		//Body--------###
		if (screenInterpreter.screenToMapElements(lastScreen, world, internalCellOffset))
		{
			Point globalOffset = screenInterpreter.getGlobalWindowOffset();
			globalOffsetX = globalOffset.x;
			globalOffsetY = globalOffset.y;

			SetEvent(hEvent_StitchMapThread);
		}
		else
			SetEvent(hEvent_ScreenCaptureThread);
		//Body--------###

		//ORiUtils::ConsoleLogTimed("World", start);
		ResetEvent(hEvent_ScreenInterpreterThread);
		waitKey(1);
	}
	shouldMappingTerminate = true;

	shouldGameTerminate = true;
}

void reactToWorld(vector<vector<MapElement*>>& world, Point2f InternalCellOffset)
{
	Environment environment(world);
	inputEmulator.setGlobalOffset(globalOffsetX, globalOffsetY);

	if (environment.mobiles.size() > 0)
		//Attack mobs in view
	{
		inputEmulator.PressKey(VK_DOWN);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		inputEmulator.PressKey(VK_RETURN);
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		Doll_Actions::getAction(environment.mobiles.back().name)(inputEmulator, environment.mobiles.back().pos, InternalCellOffset);
	}
	else if(environment.countIgnores(Black) == 0)
	{

		Doll_Actions::Teleport(inputEmulator, Point(0,0), InternalCellOffset);
		//cout << "NOTHING!";
	}
}

void StitchMapThread()
{
	if (!hEvent_StitchMapThread) assert(false);
	std::future<vector<Point>> path = std::async(std::launch::async, FindPath, mapStitcher.getGridMap(), mapStitcher.getMyGridPos());

	while (!shouldMappingTerminate)
	{
		WaitForSingleObject(hEvent_StitchMapThread, INFINITE);
		vector<vector<MapElement*>> lastWorld = world;
		Point2f lastInternalCellOffset = internalCellOffset;
		SetEvent(hEvent_ScreenCaptureThread);
		//Body--------###

		clock_t start = clock();

			if (mapStitcher.appendToMap(lastWorld))
			{
				Environment environment(lastWorld);
				inputEmulator.setGlobalOffset(globalOffsetX, globalOffsetY);

				if (environment.mobiles.size() > 0)
					//Attack mobs in view
				{
					cout << "Action!";
					Doll_Actions::getAction(environment.mobiles.back().name)(inputEmulator, environment.mobiles.back().pos, lastInternalCellOffset);
				}
				else
					//Do Path finding
					if (mapStitcher.didGridGrow() || route.size() == 0)
					{
						if (pathfindingDone)
						{
							route = path.get();
							path = std::async(std::launch::async, FindPath, mapStitcher.getGridMap(), mapStitcher.getMyGridPos());
							pathfindingDone = false;
						}

						//pathFindingTHreadCopiedGrid = false;
					}
					else if (route.size() > 0)
					{
						inputEmulator.followRoute(route, mapStitcher.getMyGridPos(), lastInternalCellOffset);
					}
				ORiUtils::ConsoleLogTimed("Map", start);
			}
			else
				inputEmulator.ReleaseLeftClick();


		ResetEvent(hEvent_StitchMapThread);

		//Body--------###
		waitKey(1);
	}
	shouldGameTerminate = true;
}

vector<Point> FindPath(deque<deque<MapTile>> gridMap, Point DolPoss)
{
		//Body--------###
		clock_t start = clock();
		vector<Point> routeF = navigator.doPathFinding(gridMap, DolPoss);
		ORiUtils::ConsoleLogTimed("Pathing", start); 
		pathfindingDone = true;
		waitKey(1);
		return routeF;
		//Body--------###
}
