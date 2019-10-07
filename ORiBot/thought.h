#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include "mapElement.h"

using namespace std;
using namespace cv;

class Thought
{
	MapElementCollection mapElementCollection;
public:
	deque<deque<MapElement*>> gridMap;
	Point lastGridPos = Point(-1, -1);

	Thought() {};
	void lookAtMapConents(vector<vector<MapElement*>>& world);
	void appendToMap(vector<vector<MapElement*>>& mapPiece);
private:
	void drawMap(deque<deque<MapElement*>>& map, string windowName);
	Point StitchMap(int foundRow, int foundCol, vector<vector<MapElement*>>& mapPiece);
	void compareCenter(deque<deque<MapElement*>>& gridMap, vector<vector<MapElement*>>& mapPiece, int foundRow, int foundCol, const int C_rowStart, const int C_colStart, const int C_rowSize, const int C_colSize, const function<int(int, int)>& pPartRow_CC, const function<int(int, int)>& pPartCol_CC, const function<int(int, int)>& gPartRow_CC, const function<int(int, int)>& gPartCol_CC, int& gridSearchLoopCount);
	deque<deque<MapElement*>> addToTop(deque<deque<MapElement*>>& gridMap, vector<vector<MapElement*>>& mapPiece, int foundRow, int foundCol, const int Top_rowStart, const int Top_colStart, const int Top_rowSize, const int Top_colSize, const function<int(int, int)>& pPartRow_Top, const function<int(int, int)>& pPartCol_Top, const function<int(int, int)>& gPartRow_Top, const function<int(int, int)>& gPartCol_Top, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Top, int& gridSearchLoopCount);
	deque<deque<MapElement*>> addToBottom(deque<deque<MapElement*>>& gridMap, vector<vector<MapElement*>>& mapPiece, int foundRow, int foundCol, const int Bottom_rowStart, const int Bottom_colStart, const int Bottom_rowSize, const int Bottom_colSize, const function<int(int, int)>& pPartRow_Bottom, const function<int(int, int)>& pPartCol_Bottom, const function<int(int, int)>& gPartRow_Bottom, const function<int(int, int)>& gPartCol_Bottom, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Bottom, int& gridSearchLoopCount);
	void addToLeft(deque<deque<MapElement*>>& gridMap, vector<vector<MapElement*>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount);
	void addToRight(deque<deque<MapElement*>>& gridMap, vector<vector<MapElement*>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount);

};

