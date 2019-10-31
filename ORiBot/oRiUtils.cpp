#include "oRiUtils.h"

//staticStuff
const vector<vector<Point2f>> ORiUtils::cellPositionalMap = generateCellPositionalMap();
mutex ORiUtils::mtx;

void ORiUtils::draw(deque<deque<MapTile>>& image, const string windowName, const int resize)
{
	Mat mapImg(image.size(), image.front().size(), CV_8UC3);
	for (int row = 0; row < image.size(); row++)
		for (int col = 0; col < image.front().size(); col++)
		{
				mapImg.at<Vec3b>(Point(col, row)) = image.at(row).at(col).mapElement->color;
		}
	if(resize > 1)
	cv::resize(mapImg, mapImg, cv::Size(), resize, resize, INTER_NEAREST);

	imshow(windowName, mapImg);
};


double ORiUtils::getDistance(Point pointA, Point pointB)
{
	return double(sqrt((pointA.x - pointB.x) * (pointA.x - pointB.x) + (pointA.y - pointB.y) * (pointA.y - pointB.y)));
}


void ORiUtils::ConsoleLogTimed(string name, clock_t start)
{
	mtx.lock();
	double time_taken = double((clock() - start) / double(CLOCKS_PER_SEC));
	cout << "|" << name << " " << fixed << time_taken * 1000 << setprecision(0); cout << "ms ";
	cout << fixed << 1 / time_taken << setprecision(1); cout << "FPS|";
	cout << endl;
	mtx.unlock();
}

void ORiUtils::ConsoleLog(string log)
{
	mtx.lock();
	cout << log;
	cout << endl;
	mtx.unlock();
}

Point ORiUtils::CellPixelPosition(int row, int col)
{
	try {
		return ORiUtils::cellPositionalMap.at(row).at(col);
	}
	catch (...) {
		ORiUtils::ConsoleLog("[ERROR] CellPosMap out of bounds");
		return Point(-1, -1);
	}
}

Point ORiUtils::CellPixelPositionByOffset(int rowOffset, int colOffset)
{
	try {
		int row = ((cellPositionalMap.size()) / 2) + rowOffset;
		int col = (cellPositionalMap.front().size() / 2) + colOffset;
		return ORiUtils::cellPositionalMap.at(row).at(col);
	}
	catch (...) {
		ORiUtils::ConsoleLog("[ERROR] CellPosMap out of bounds");
		return Point(-1, -1);
	}
}
