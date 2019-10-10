#include "mapStitcher.h"

void MapStitcher::appendToMap(vector<vector<const MapElement*>>& mapPiece)
{
	const int border = 3;

	if (gridMap.size() == 0)
	{
		for (vector<const MapElement*> mapPieceLine : mapPiece)
		{
			deque<const MapElement*>gridLine;
			for (const MapElement* mapElement : mapPieceLine)
				gridLine.push_back(mapElement);
			gridMap.push_back(gridLine);
		}
		return;
	}
	else
	{
		//Look for match
		clock_t startMatch = clock();
		int matchLoopOuterCount = 0;
		int matchLoopInnerCount = 0;
		int negativeMatch = 0;

		int xStart, yStart, xEnd, yEnd;

		if (lastGridPos.x != -1 || lastGridPos.y != -1)
		{
			xStart = lastGridPos.x; xStart -= (border + 1); if (xStart < 0) xStart = 0;
			yStart = lastGridPos.y;  yStart -= (border + 1); if (yStart < 0) yStart = 0;
			xEnd = lastGridPos.x + border + 1;
			yEnd = lastGridPos.y + border + 1;
			lastGridPos = Point(-1, -1);
		}
		else
		{
			xStart = 0;
			yStart = 0;
			xEnd = gridMap.size();
			yEnd = gridMap.front().size();
		}

		//cout << "Start" << "(" << xStart << "," << yStart << ") ";
		//cout << "End" << "(" << xEnd << "," << yEnd << ") ";
		for (int foundRow = xStart; foundRow < gridMap.size() - mapPiece.size() + border * 2 + 1 && foundRow < xEnd; foundRow++)
			for (int foundCol = yStart; foundCol < gridMap.front().size() - mapPiece.front().size() + border * 2 + 1 && foundCol < yEnd; foundCol++)
			{
				matchLoopOuterCount++;
				if (gridMap.at(foundRow).at(foundCol)->name == mapPiece.at(border).at(border)->name || gridMap.at(foundRow).at(foundCol)->type == 1 || mapPiece.at(border).at(border)->type == 1)
				{
					negativeMatch = 0;
					for (int row = 1; ((row + foundRow) < gridMap.size() && row < (mapPiece.size() - border * 2)); row++)
					{
						for (int col = 1; ((col + foundCol) < gridMap.front().size() && col < (mapPiece.front().size() - border * 2)); col++)
						{
							matchLoopInnerCount++;
							if (!(gridMap.at((row + foundRow)).at((col + foundCol))->name == mapPiece.at(row + border).at(col + border)->name ||
								(gridMap.at((row + foundRow)).at((col + foundCol))->name == "WalkableA" && mapPiece.at(row + border).at(col + border)->name == "WalkableB") ||
								(gridMap.at((row + foundRow)).at((col + foundCol))->name == "WalkableB" && mapPiece.at(row + border).at(col + border)->name == "WalkableA") ||
								gridMap.at((row + foundRow)).at((col + foundCol))->type == 1 ||
								mapPiece.at(row + border).at(col + border)->type == 1))
							{
								negativeMatch++;
								if (negativeMatch > 1)
								{
									goto nextGrid;
								}

							}
						}
					}
					//Append to Grid Map
					//cout << "Match Search Loops: " << matchLoopOuterCount << " + " << matchLoopInnerCount << " @ " << fixed << double((clock() - startMatch) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";
					lastGridPos = StitchMap(foundRow - border, foundCol - border, mapPiece);
					lastGridPos += Point(border, border);
					return;
				}

			nextGrid:;
			}
		//cout << "No Matches: " << matchLoopOuterCount << " + " << matchLoopInnerCount << " @ " << fixed << double((clock() - startMatch) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";
	}
};

Point MapStitcher::StitchMap(int foundRow, int foundCol, vector<vector<const MapElement*>>& mapPiece)
{
	int gridSearchLoopCount = 0;

	const int GridSartRows = gridMap.size();
	const int GridSartCols = gridMap.front().size();

	clock_t startGridLook = clock();
	//cout << "Found (" << foundRow << "," << foundCol << ") ";

	//1A
	if (foundRow < 0 && foundCol < 0)
	{
		//cout << " 1A ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row + foundRow; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col + foundCol; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row < mapPieceSize && col < mapPieceFrontSize); };

		//CC
		const int C_rowStart = -foundRow;
		const int C_colStart = -foundCol;
		const int C_rowSize = mapPiece.size();
		const int C_colSize = mapPiece.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Top
		const int Top_rowStart = 0;
		const int Top_colStart = 0;
		const int Top_rowSize = -foundRow;
		const int Top_colSize = gridMap.front().size() - foundCol;
		deque<deque<const MapElement*>> Top_mapBlock = addToTop(gridMap, mapPiece, foundRow, foundCol, Top_rowStart, Top_colStart, Top_rowSize, Top_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);


		//Left
		const int Left_rowStart = -foundRow;
		const int Left_colStart = 0;
		const int Left_rowSize = -foundRow + gridMap.size();
		const int Left_colSize = -foundCol + 1;
		addToLeft(gridMap, mapPiece, foundRow, foundCol, Left_rowStart, Left_colStart, Left_rowSize, Left_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);

		//Top +
		for (int i = 0; i < Top_mapBlock.size(); i++)
			gridMap.push_front(Top_mapBlock.at(i));
	}
	//1B
	else if (foundRow < 0 && foundCol >= 0 &&
		foundCol + mapPiece.front().size() <= gridMap.front().size())
	{
		//cout << " 1B ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row + foundRow; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row < -foundRow && col >= foundCol && col < foundCol + mapPieceFrontSize); };

		//CC
		const int C_rowStart = -foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = mapPiece.size();
		const int C_colSize = foundCol + mapPiece.front().size(); compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Top
		const int Top_rowStart = 0;
		const int Top_colStart = 0;
		const int Top_rowSize = -foundRow;
		const int Top_colSize = gridMap.front().size();
		deque<deque<const MapElement*>> Top_mapBlock = addToTop(gridMap, mapPiece, foundRow, foundCol, Top_rowStart, Top_colStart, Top_rowSize, Top_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

		//Top +
		for (int i = 0; i < Top_mapBlock.size(); i++)
			gridMap.push_front(Top_mapBlock.at(i));
	}
	//1C
	else if (foundRow < 0 && foundCol > 0 &&
		foundCol + mapPiece.front().size() >= gridMap.front().size())
	{
		//cout << " 1C ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row + foundRow; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row < mapPieceSize && col >= foundCol); };

		//CC
		const int C_rowStart = -foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = mapPiece.size();
		const int C_colSize = gridMap.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Right
		const int Right_rowStart = -foundRow;
		const int Right_colStart = gridMap.front().size();
		const int Right_rowSize = -foundRow + gridMap.size();
		const int Right_colSize = foundCol + mapPiece.front().size();
		addToRight(gridMap, mapPiece, foundRow, foundCol, Right_rowStart, Right_colStart, Right_rowSize, Right_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);

		//Top
		const int Top_rowStart = 0;
		const int Top_colStart = 0;
		const int Top_rowSize = -foundRow;
		const int Top_colSize = mapPiece.front().size() + foundCol;
		deque<deque<const MapElement*>> Top_mapBlock = addToTop(gridMap, mapPiece, foundRow, foundCol, Top_rowStart, Top_colStart, Top_rowSize, Top_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);


		//Top +
		for (int i = 0; i < Top_mapBlock.size(); i++)
			gridMap.push_front(Top_mapBlock.at(i));
	}
	//2A
	else if (foundRow >= 0 && foundCol < 0 &&
		foundRow + mapPiece.size() <= gridMap.size())
	{
		//cout << " 2A ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col + foundCol; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col < mapPieceFrontSize && row < foundRow + mapPieceSize); };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = -foundCol;
		const int C_rowSize = foundRow + mapPiece.size();
		const int C_colSize = mapPiece.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Left
		const int Left_rowStart = 0;
		const int Left_colStart = 0;
		const int Left_rowSize = gridMap.size();
		const int Left_colSize = -foundCol + 1;
		addToLeft(gridMap, mapPiece, foundRow, foundCol, Left_rowStart, Left_colStart, Left_rowSize, Left_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);
	}
	//2B
	else if (foundRow >= 0 && foundCol >= 0 &&
		foundRow + mapPiece.size() <= gridMap.size() &&
		foundCol + mapPiece.front().size() <= gridMap.front().size())
	{
		//cout << " 2B ";
		const function<const int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<const int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<const int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<const int(int, int)> gPartCol = [](int col, int foundCol) { return col; };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = foundRow + mapPiece.size();
		const int C_colSize = foundCol + mapPiece.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);
	}
	//2C
	else if (foundRow >= 0 && foundCol > 0 &&
		foundRow + mapPiece.size() <= gridMap.size() &&
		foundCol + mapPiece.front().size() > gridMap.front().size())
	{
		//cout << " 2C ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col >= foundCol && row < foundRow + mapPieceSize); };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = foundRow + mapPiece.size();
		const int C_colSize = gridMap.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Right
		const int Right_rowStart = 0;
		const int Right_colStart = gridMap.front().size();
		const int Right_rowSize = gridMap.size();
		const int Right_colSize = foundCol + mapPiece.front().size();
		addToRight(gridMap, mapPiece, foundRow, foundCol, Right_rowStart, Right_colStart, Right_rowSize, Right_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);

	}
	//3A
	else if (foundRow > 0 && foundCol < 0 &&
		foundRow + mapPiece.size() > gridMap.size())
	{
		//cout << " 3A ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col + foundCol; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col < mapPieceFrontSize); };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = -foundCol;
		const int C_rowSize = gridMap.size();
		const int C_colSize = mapPiece.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);


		//Bottom
		const int Bottom_rowStart = gridMap.size();
		const int Bottom_colStart = 0;
		const int Bottom_rowSize = foundRow + mapPiece.size();
		const int Bottom_colSize = -foundCol + gridMap.front().size();
		deque<deque<const MapElement*>> Bottom_mapBlock = addToBottom(gridMap, mapPiece, foundRow, foundCol, Bottom_rowStart, Bottom_colStart, Bottom_rowSize, Bottom_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

		//Left
		const int Left_rowStart = 0;
		const int Left_colStart = 0;
		const int Left_rowSize = gridMap.size();
		const int Left_colSize = -foundCol + 1;
		addToLeft(gridMap, mapPiece, foundRow, foundCol, Left_rowStart, Left_colStart, Left_rowSize, Left_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);

		//Bottom +
		for (int i = 0; i < Bottom_mapBlock.size(); i++)
			gridMap.push_back(Bottom_mapBlock.at(i));
	}
	//3B
	else if (foundRow > 0 && foundCol >= 0 &&
		foundCol + mapPiece.front().size() <= gridMap.front().size() &&
		foundRow + mapPiece.size() > gridMap.size())
	{
		//cout << " 3B ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col >= foundCol && col < foundCol + mapPieceFrontSize); };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = gridMap.size();
		const int C_colSize = foundCol + mapPiece.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Bottom
		const int Bottom_rowStart = gridMap.size();
		const int Bottom_colStart = 0;
		const int Bottom_rowSize = foundRow + mapPiece.size();
		const int Bottom_colSize = gridMap.front().size();
		deque<deque<const MapElement*>> Bottom_mapBlock = addToBottom(gridMap, mapPiece, foundRow, foundCol, Bottom_rowStart, Bottom_colStart, Bottom_rowSize, Bottom_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

		//Bottom +
		for (int i = 0; i < Bottom_mapBlock.size(); i++)
			gridMap.push_back(Bottom_mapBlock.at(i));
	}
	//3C
	else if (foundRow > 0 && foundCol > 0 &&
		foundCol + mapPiece.front().size() > gridMap.front().size() &&
		foundRow + mapPiece.size() > gridMap.size())
	{
		//cout << " 3C ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col; };
		const function<bool(int, int, int, int, const int, const int)> pPartCod = [](int row, int foundRow, int col, int foundCol, const int mapPieceSize, const int mapPieceFrontSize) { return (row >= foundRow && col >= foundCol); };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = gridMap.size();
		const int C_colSize = gridMap.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);

		//Bottom
		const int Bottom_rowStart = gridMap.size();
		const int Bottom_colStart = 0;
		const int Bottom_rowSize = foundRow + mapPiece.size();
		const int Bottom_colSize = foundCol + mapPiece.front().size();
		deque<deque<const MapElement*>> Bottom_mapBlock = addToBottom(gridMap, mapPiece, foundRow, foundCol, Bottom_rowStart, Bottom_colStart, Bottom_rowSize, Bottom_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

		//Right
		const int Right_rowStart = 0;
		const int Right_colStart = gridMap.front().size();
		const int Right_rowSize = gridMap.size();
		const int Right_colSize = foundCol + mapPiece.front().size();
		addToRight(gridMap, mapPiece, foundRow, foundCol, Right_rowStart, Right_colStart, Right_rowSize, Right_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);

		//Bottom +
		for (int i = 0; i < Bottom_mapBlock.size(); i++)
			gridMap.push_back(Bottom_mapBlock.at(i));
	}
	drawMap(gridMap, "grid");
	//cout << "Grid Search Loops: " << gridSearchLoopCount << " @ " << fixed << double((clock() - startGridLook) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";

	int R = gridMap.size() - GridSartRows;
	int C = gridMap.front().size() - GridSartCols;

	if (foundRow >= 0) R = 0;
	if (foundCol >= 0) C = 0;
	return Point(foundRow + R, foundCol + C);
}

void MapStitcher::drawMap(deque<deque<const MapElement*>>& map, const string windowName)
{
	Mat mapImg = Mat::zeros(map.size(), map.front().size(), CV_8UC3);
	for (int row = 0; row < map.size(); row++)
		for (int col = 0; col < map.front().size(); col++)
		{
			mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col)->color;
		}
	//cv::resize(mapImg, mapImg, cv::Size(), 6, 6, INTER_NEAREST);
	imshow(windowName, mapImg);
};

void MapStitcher::compareCenter(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int C_rowStart, const int C_colStart, const int C_rowSize, const int C_colSize, const function<int(int, int)>& pPartRow_CC, const function<int(int, int)>& pPartCol_CC, const function<int(int, int)>& gPartRow_CC, const function<int(int, int)>& gPartCol_CC, int& gridSearchLoopCount)
{
	for (int row = C_rowStart; row < C_rowSize; row++)
	{
		for (int col = C_colStart; col < C_colSize; col++)
		{
			gridSearchLoopCount++;
			if (mapPiece.at(pPartRow_CC(row, foundRow)).at(pPartCol_CC(col, foundCol))->type == 0)
				gridMap.at(gPartRow_CC(row, foundRow)).at(gPartCol_CC(col, foundCol)) = (mapPiece.at(pPartRow_CC(row, foundRow)).at(pPartCol_CC(col, foundCol)));
		}
	}
}

deque<deque<const MapElement*>> MapStitcher::addToTop(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Top_rowStart, const int Top_colStart, const int Top_rowSize, const int Top_colSize, const function<int(int, int)>& pPartRow_Top, const function<int(int, int)>& pPartCol_Top, const function<int(int, int)>& gPartRow_Top, const function<int(int, int)>& gPartCol_Top, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Top, int& gridSearchLoopCount)
{
	deque<deque<const MapElement*>> Top_mapBlock;
	for (int row = Top_rowStart; row < Top_rowSize; row++)
	{
		deque<const MapElement*> mapLine;
		for (int col = Top_colStart; col < Top_colSize; col++)
		{
			gridSearchLoopCount++;

			if (pPartCod_Top(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Top(row, foundRow)).at(pPartCol_Top(col, foundCol))->type == 0)
					mapLine.push_back(mapPiece.at(pPartRow_Top(row, foundRow)).at(pPartCol_Top(col, foundCol)));
				else
					mapLine.push_back(mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255)));
			}
			else
				mapLine.push_back(mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255)));
		}
		Top_mapBlock.push_front(mapLine);
	}

	return Top_mapBlock;
}

deque<deque<const MapElement*>> MapStitcher::addToBottom(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Bottom_rowStart, const int Bottom_colStart, const int Bottom_rowSize, const int Bottom_colSize, const function<int(int, int)>& pPartRow_Bottom, const function<int(int, int)>& pPartCol_Bottom, const function<int(int, int)>& gPartRow_Bottom, const function<int(int, int)>& gPartCol_Bottom, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Bottom, int& gridSearchLoopCount)
{
	deque<deque<const MapElement*>> Bottom_mapBlock;
	for (int row = Bottom_rowStart; row < Bottom_rowSize; row++)
	{
		deque<const MapElement*> mapLine;
		for (int col = Bottom_colStart; col < Bottom_colSize; col++)
		{
			gridSearchLoopCount++;

			if (pPartCod_Bottom(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Bottom(row, foundRow)).at(pPartCol_Bottom(col, foundCol))->type == 0)
					mapLine.push_back(mapPiece.at(pPartRow_Bottom(row, foundRow)).at(pPartCol_Bottom(col, foundCol)));
				else
					mapLine.push_back(mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255)));
			}
			else
				mapLine.push_back(mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255)));
		}
		if (foundRow < 0)
			Bottom_mapBlock.push_front(mapLine);
		else
			Bottom_mapBlock.push_back(mapLine);
	}

	return Bottom_mapBlock;
}

void MapStitcher::addToLeft(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount)
{
	for (int row = Left_rowStart; row < Left_rowSize; row++)
	{
		for (int col = Left_colSize - 1; col > Left_colStart; col--)
		{
			gridSearchLoopCount++;
			const MapElement* mapElementToAppend;
			if (pPartCod_Left(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Left(row, foundRow)).at(pPartCol_Left(col, foundCol))->type == 0)
					mapElementToAppend = (mapPiece.at(pPartRow_Left(row, foundRow)).at(pPartCol_Left(col, foundCol)));
				else
					mapElementToAppend = mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255));
			}
			else
				mapElementToAppend = mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255));

			gridMap.at(gPartRow_Left(row, foundRow)).push_front(mapElementToAppend);
		}
	}
}

void MapStitcher::addToRight(deque<deque<const MapElement*>>& gridMap, vector<vector<const MapElement*>>& mapPiece, int foundRow, int foundCol, const int Right_rowStart, const int Right_colStart, const int Right_rowSize, const int Right_colSize, const function<int(int, int)>& pPartRow_Right, const function<int(int, int)>& pPartCol_Right, const function<int(int, int)>& gPartRow_Right, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Right, int& gridSearchLoopCount)
{
	for (int row = Right_rowStart; row < Right_rowSize; row++)
	{
		for (int col = Right_colStart; col < Right_colSize; col++)
		{
			gridSearchLoopCount++;
			const MapElement* mapElementToAppend;
			if (pPartCod_Right(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Right(row, foundRow)).at(pPartCol_Right(col, foundCol))->type == 0)
					mapElementToAppend = (mapPiece.at(pPartRow_Right(row, foundRow)).at(pPartCol_Right(col, foundCol)));
				else
					mapElementToAppend = mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255));
			}
			else
				mapElementToAppend = mapElementCollection.searchMapElementByColor(Vec3b(0, 255, 255));

			gridMap.at(gPartRow_Right(row, foundRow)).push_back(mapElementToAppend);
		}
	}
}