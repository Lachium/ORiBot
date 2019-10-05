#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include "mob.h"

using namespace std;
using namespace cv;

class Thought
{
	MapElementCollection mapElementCollection;
public:
	deque<deque<MapElement>> gridMap;
	Point lastGridPos = Point(-1,-1);

	Thought() {};
	void lookAtMapConents(Mat& world);
	void appendToMap(deque<deque<MapElement>> & mapPiece);
	MapElement innerConditions(const bool pPartCod, const bool gPartCod, const int pPartRow, const int pPartCol, const int gPartRow, const int gPartCol, deque<deque<MapElement>>& mapPiece, deque<deque<MapElement>>& gridMap);
	void drawMap(deque<deque<MapElement>>& map, string windowName);
	void StitchMap(int foundRow, int foundCol, deque<deque<MapElement>> & mapPiece);
	void compareCenter(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int C_rowStart, const int C_colStart, const int C_rowSize, const int C_colSize, const function<int(int, int)>& pPartRow_CC, const function<int(int, int)>& pPartCol_CC, const function<int(int, int)>& gPartRow_CC, const function<int(int, int)>& gPartCol_CC, int& gridSearchLoopCount);
	deque<deque<MapElement>> addToTop(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Top_rowStart, const int Top_colStart, const int Top_rowSize, const int Top_colSize, const function<int(int, int)>& pPartRow_Top, const function<int(int, int)>& pPartCol_Top, const function<int(int, int)>& gPartRow_Top, const function<int(int, int)>& gPartCol_Top, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Top, int& gridSearchLoopCount);
	deque<deque<MapElement>> addToBottom(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Bottom_rowStart, const int Bottom_colStart, const int Bottom_rowSize, const int Bottom_colSize, const function<int(int, int)>& pPartRow_Bottom, const function<int(int, int)>& pPartCol_Bottom, const function<int(int, int)>& gPartRow_Bottom, const function<int(int, int)>& gPartCol_Bottom, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Bottom, int& gridSearchLoopCount);
	void addToLeft(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount);
	void addToRight(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount);

	};

