#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include "mapElement.h"

using namespace std;
using namespace cv;

class MapStitcher
{
	MapElementCollection mapElementCollection;
public:
	MapStitcher() {};
	void appendToMap(vector<vector<const MapElement*>>& mapPiece);
private:
	deque<deque<const MapElement*>> gridMap;
	Point lastGridPos = Point(-1, -1);
	void drawMap(deque<deque<const MapElement*>>& map, const string windowName) const;
	Point StitchMap(int foundRow, int foundCol, vector<vector<const MapElement*>>& mapPiece);
	void compareCenter(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int C_rowStart, const int C_colStart, const int C_rowSize, const int C_colSize, const function<int(int, int)>& pPartRow_CC, const function<int(int, int)>& pPartCol_CC, const function<int(int, int)>& gPartRow_CC, const function<int(int, int)>& gPartCol_CC, int& gridSearchLoopCount);
	deque<deque<const MapElement*>> addToTop(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Top_rowStart, const int Top_colStart, const int Top_rowSize, const int Top_colSize, const function<int(int, int)>& pPartRow_Top, const function<int(int, int)>& pPartCol_Top, const function<int(int, int)>& gPartRow_Top, const function<int(int, int)>& gPartCol_Top, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Top, int& gridSearchLoopCount);
	deque<deque<const MapElement*>> addToBottom(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Bottom_rowStart, const int Bottom_colStart, const int Bottom_rowSize, const int Bottom_colSize, const function<int(int, int)>& pPartRow_Bottom, const function<int(int, int)>& pPartCol_Bottom, const function<int(int, int)>& gPartRow_Bottom, const function<int(int, int)>& gPartCol_Bottom, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Bottom, int& gridSearchLoopCount);
	void addToLeft(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount);
	void addToRight(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Right_rowStart, const int Right_colStart, const int Right_rowSize, const int Right_colSize, const function<int(int, int)>& pPartRow_Right, const function<int(int, int)>& pPartCol_Right, const function<int(int, int)>& gPartRow_Right, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Right, int& gridSearchLoopCount);

	//Lamda Functions
	//1A
	const function<int(int, int)> pPartRow_1A = [](int row, int foundRow) { return row; };
	const function<int(int, int)> pPartCol_1A = [](int col, int foundCol) { return col; };
	const function<int(int, int)> gPartRow_1A = [](int row, int foundRow) { return row + foundRow; };
	const function<int(int, int)> gPartCol_1A = [](int col, int foundCol) { return col + foundCol; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_1A = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row < mapPieceSize && col < mapPieceFrontSize); };
	//1B
	const function<int(int, int)> pPartRow_1B = [](int row, int foundRow) { return row; };
	const function<int(int, int)> pPartCol_1B = [](int col, int foundCol) { return col - foundCol; };
	const function<int(int, int)> gPartRow_1B = [](int row, int foundRow) { return row + foundRow; };
	const function<int(int, int)> gPartCol_1B = [](int col, int foundCol) { return col; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_1B = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row < -foundRow && col >= foundCol && col < foundCol + mapPieceFrontSize); };
	//1C
	const function<int(int, int)> pPartRow_1C = [](int row, int foundRow) { return row; };
	const function<int(int, int)> pPartCol_1C = [](int col, int foundCol) { return col - foundCol; };
	const function<int(int, int)> gPartRow_1C = [](int row, int foundRow) { return row + foundRow; };
	const function<int(int, int)> gPartCol_1C = [](int col, int foundCol) { return col; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_1C = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row < mapPieceSize && col >= foundCol); };
	//2A
	const function<int(int, int)> pPartRow_2A = [](int row, int foundRow) { return row - foundRow; };
	const function<int(int, int)> pPartCol_2A = [](int col, int foundCol) { return col; };
	const function<int(int, int)> gPartRow_2A = [](int row, int foundRow) { return row; };
	const function<int(int, int)> gPartCol_2A = [](int col, int foundCol) { return col + foundCol; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_2A = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col < mapPieceFrontSize && row < foundRow + mapPieceSize); };
	//2B
	const function<const int(int, int)> pPartRow_2B = [](int row, int foundRow) { return row - foundRow; };
	const function<const int(int, int)> pPartCol_2B = [](int col, int foundCol) { return col - foundCol; };
	const function<const int(int, int)> gPartRow_2B = [](int row, int foundRow) { return row; };
	const function<const int(int, int)> gPartCol_2B = [](int col, int foundCol) { return col; };
	//2C
	const function<int(int, int)> pPartRow_2C = [](int row, int foundRow) { return row - foundRow; };
	const function<int(int, int)> pPartCol_2C = [](int col, int foundCol) { return col - foundCol; };
	const function<int(int, int)> gPartRow_2C = [](int row, int foundRow) { return row; };
	const function<int(int, int)> gPartCol_2C = [](int col, int foundCol) { return col; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_2C = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col >= foundCol && row < foundRow + mapPieceSize); };
	//3A
	const function<int(int, int)> pPartRow_3A = [](int row, int foundRow) { return row - foundRow; };
	const function<int(int, int)> pPartCol_3A = [](int col, int foundCol) { return col; };
	const function<int(int, int)> gPartRow_3A = [](int row, int foundRow) { return row; };
	const function<int(int, int)> gPartCol_3A = [](int col, int foundCol) { return col + foundCol; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_3A = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col < mapPieceFrontSize); };
	//3B
	const function<int(int, int)> pPartRow_3B = [](int row, int foundRow) { return row - foundRow; };
	const function<int(int, int)> pPartCol_3B = [](int col, int foundCol) { return col - foundCol; };
	const function<int(int, int)> gPartRow_3B = [](int row, int foundRow) { return row; };
	const function<int(int, int)> gPartCol_3B = [](int col, int foundCol) { return col; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod_3B = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col >= foundCol && col < foundCol + mapPieceFrontSize); };
	//3C
	const function<int(int, int)> pPartRow_3C = [](int row, int foundRow) { return row - foundRow; };
	const function<int(int, int)> pPartCol_3C = [](int col, int foundCol) { return col - foundCol; };
	const function<int(int, int)> gPartRow_3C = [](int row, int foundRow) { return row; };
	const function<int(int, int)> gPartCol_3C = [](int col, int foundCol) { return col; };
	const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col >= foundCol); };
};

