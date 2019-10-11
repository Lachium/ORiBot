#include <opencv2/opencv.hpp>
#include "screenInterpreter.h"
#include "mapStitcher.h"
#include "mapElement.h"
#include <atomic>

using namespace std;
using namespace cv;

void StitchMapThread(void);
void ScreenCaptureThread(void);
atomic<bool> shouldGameTerminate = false;
atomic<bool> shouldScreenCaptureTerminate = false;
atomic<bool> shouldMappingTerminate = false;

Mat lastScreen;
vector<vector<const MapElement*>> lastWorld;

int main(int argv, char** argc)
{
	HANDLE hEvent_ScreenCaptureThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_CAPTURE");
	thread screenCaptureThread(ScreenCaptureThread);

	ScreenCapture screenCapture;
	

	int key = 0;
	while (!shouldGameTerminate)
	{
		clock_t start = clock();

		Mat * screen = screenCapture.readImage();
		lastScreen = *screen;
		SetEvent(hEvent_ScreenCaptureThread);
		
		double time_taken = double((clock() - start) / double(CLOCKS_PER_SEC));
		cout << "|Capture " << fixed << time_taken * 1000 << setprecision(0); cout << "ms ";
		cout << fixed << 1 / time_taken << setprecision(1); cout << "FPS|";
		cout << endl;
		//key = waitKey(1);
	}
	shouldScreenCaptureTerminate = true;
	screenCaptureThread.join();
	waitKey(0);
	return 0;
}

void ScreenCaptureThread()
{
	ScreenInterpreter screenInterpreter = ScreenInterpreter();
	HANDLE hEvent_ScreenCaptureThread = CreateEvent(NULL, true, false, L"FIRE_SCREEN_CAPTURE");

	if (!hEvent_ScreenCaptureThread)
	{
		assert(false);
	}

	vector<vector<const MapElement*>> world;
	thread stictMapThread(StitchMapThread);
	HANDLE hEvent_StitchMapThread = CreateEvent(NULL, true, false, L"FIRE_STITCH_MAP");

	int key = 0;
	while (!shouldScreenCaptureTerminate)
	{
		WaitForSingleObject(hEvent_ScreenCaptureThread, INFINITE);

		clock_t start = clock();
		if (screenInterpreter.screenToMapElements(lastScreen, world))
		{
			lastWorld = world;
			SetEvent(hEvent_StitchMapThread);
		}

		cout << "|World " << fixed << double((clock() - start) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms|" << endl;
		ResetEvent(hEvent_ScreenCaptureThread);
		//key = waitKey(1);
	}
	shouldMappingTerminate = true;
	stictMapThread.join();

	shouldGameTerminate = true;
}

void StitchMapThread()
{
	MapStitcher mapStitcher = MapStitcher();
	HANDLE hEvent_StitchMapThread = CreateEvent(NULL, true, false, L"FIRE_STITCH_MAP");

	if (!hEvent_StitchMapThread)
	{
		assert(false);
	}

	int key = 0;
	while (!shouldMappingTerminate)
	{
		WaitForSingleObject(hEvent_StitchMapThread, INFINITE);

		clock_t start = clock();
		mapStitcher.appendToMap(lastWorld);
		cout << "|Map " << fixed << double((clock() - start) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms|" << endl;
		ResetEvent(hEvent_StitchMapThread);
		key = waitKey(1);
	}
	shouldGameTerminate = true;
}
