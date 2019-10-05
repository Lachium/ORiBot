#include "thought.h"


void Thought::lookAtMapConents(Mat& world)
{
	//Mat eyes = Mat::zeros(world.rows, world.cols, CV_8UC4);

	//Mapping
	deque<deque<MapElement>> viewedMapContents;
	for (int row = 0; row < world.rows; row++)
	{
		deque<MapElement> viewedMapContentsCol;
		for (int col = 0; col < world.cols; col++)
		{
			Vec4b worldColorPoint = (world.at<Vec4b>(Point(col, row)));
			viewedMapContentsCol.push_back(mapElementCollection.searchMapElementByColor(Vec3b(worldColorPoint[0], worldColorPoint[1], worldColorPoint[2])));
			Vec3b color = viewedMapContentsCol.back().color;
			//eyes.at<Vec4b>(Point(col, row)) = Vec4b(color[0], color[1], color[2], 255);
		}
		viewedMapContents.push_back(viewedMapContentsCol);
	}
	appendToMap(viewedMapContents);
	//drawMap(mapPiece, "piece");
	//drawMap(gridMap, "grid");

	//for (const auto& key : mapElementCollection.mapElements)
	//	cout << key.second.name << " " << key.second.mapCount << endl;

	mapElementCollection.clearCounts();

	//cv::resize(eyes, eyes, cv::Size(), 6, 6, INTER_NEAREST);
	//imshow("Eyes", eyes);


};

void Thought::appendToMap(deque<deque<MapElement>>& mapPiece)
{
	const int border = 3;

	if (gridMap.size() == 0)
	{
		gridMap = mapPiece;
		return;
	}
	else
	{
		//Look for match
		clock_t startMatch = clock();
		int matchSearchLoopCount = 0;
		int negativeMatch = 0;
		int searches = 1;

		int xStart, yStart, xEnd, yEnd;

		if (lastGridPos.x != -1 || lastGridPos.y != -1)
		{
			xStart = lastGridPos.x; if (xStart - searches >= 0) xStart -= searches;
			yStart = lastGridPos.y; if (yStart - searches >= 0) yStart -= searches;
			xEnd = xStart + searches;
			yEnd = yStart + searches;
		}
		else
		{
			xStart = 0;
			yStart = 0;
			xEnd = gridMap.size();
			yEnd = gridMap.front().size();
		}

		cout << "Start" << "(" << xStart << "," << yStart << ") ";
		cout << "End" << "(" << xEnd << "," << yEnd << ") ";
		for (int foundRow = xStart; foundRow < gridMap.size() - (mapPiece.size() - border * 2) && foundRow < xEnd; foundRow++)
			for (int foundCol = yStart; foundCol < gridMap.front().size() - (mapPiece.front().size() - border * 2) && foundCol < yEnd; foundCol++)
			{
				if (gridMap.at(foundRow).at(foundCol).name == mapPiece.at(border).at(border).name || gridMap.at(foundRow).at(foundCol).type == 1 || mapPiece.at(border).at(border).type == 1)
				{
					negativeMatch = 0;
					for (int row = 1; ((row + foundRow) < gridMap.size() && row < (mapPiece.size() - border * 2)); row++)
					{
						for (int col = 1; ((col + foundCol) < gridMap.front().size() && col < (mapPiece.front().size() - border * 2)); col++)
						{
							matchSearchLoopCount++;
							if (!(gridMap.at((row + foundRow)).at((col + foundCol)).name == mapPiece.at(row + border).at(col + border).name ||
								(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableA" && mapPiece.at(row + border).at(col + border).name == "WalkableB") ||
								(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableB" && mapPiece.at(row + border).at(col + border).name == "WalkableA") ||
								gridMap.at((row + foundRow)).at((col + foundCol)).type == 1 ||
								mapPiece.at(row + border).at(col + border).type == 1))
							{
								negativeMatch++;
								if (negativeMatch > 4)
								{
									cout << "Not Found ";
									searches++;
									goto nextGrid;
								}

							}
						}
					}
					//Append to Grid Map
					cout << "Match Search Loops: " << matchSearchLoopCount << " @ " << fixed << double((clock() - startMatch) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";
					StitchMap(foundRow - border, foundCol - border, mapPiece);
				}
				else
					searches++;

			nextGrid:;

				if (searches > border)
				{
					lastGridPos = Point(-1, -1);
				}
			}
	}
};

void Thought::StitchMap(int foundRow, int foundCol, deque<deque<MapElement>> & mapPiece)
{
	int gridSearchLoopCount = 0;
	clock_t startGridLook = clock();
	lastGridPos = Point(foundRow, foundCol);
	cout << "Found (" << foundRow << "," << foundCol << ") ";// << endl;
	cout << " Merge Point: " << "(" << foundRow << "," << foundCol << ") ";

	//1A
	if (foundRow < 0 && foundCol < 0)
	{
		cout << " 1A ";
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
		deque<deque<MapElement>> Top_mapBlock = addToTop(gridMap, mapPiece, foundRow, foundCol, Top_rowStart, Top_colStart, Top_rowSize, Top_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);


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
		cout << " 1B ";
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
		deque<deque<MapElement>> Top_mapBlock = addToTop(gridMap, mapPiece, foundRow, foundCol, Top_rowStart, Top_colStart, Top_rowSize, Top_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

		//Top +
		for (int i = 0; i < Top_mapBlock.size(); i++)
			gridMap.push_front(Top_mapBlock.at(i));
	}
	//1C
	else if (foundRow < 0 && foundCol > 0 &&
		foundCol + mapPiece.front().size() >= gridMap.front().size())
	{
		cout << " 1C ";
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
		deque<deque<MapElement>> Top_mapBlock = addToTop(gridMap, mapPiece, foundRow, foundCol, Top_rowStart, Top_colStart, Top_rowSize, Top_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);


		//Top +
		for (int i = 0; i < Top_mapBlock.size(); i++)
			gridMap.push_front(Top_mapBlock.at(i));
	}
	//2A
	else if (foundRow >= 0 && foundCol < 0 &&
		foundRow + mapPiece.size() <= gridMap.size())
	{
		cout << " 2A ";
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
		const int Left_colSize = -foundCol;
		addToLeft(gridMap, mapPiece, foundRow, foundCol, Left_rowStart, Left_colStart, Left_rowSize, Left_colSize, pPartRow, pPartCol, gPartRow, pPartCod, gridSearchLoopCount);
	}
	//2B
	else if (foundRow >= 0 && foundCol >= 0 &&
		foundRow + mapPiece.size() <= gridMap.size() &&
		foundCol + mapPiece.front().size() <= gridMap.front().size())
	{
		cout << " 2B ";
		const function<int(int, int)> pPartRow = [](int row, int foundRow) { return row - foundRow; };
		const function<int(int, int)> pPartCol = [](int col, int foundCol) { return col - foundCol; };
		const function<int(int, int)> gPartRow = [](int row, int foundRow) { return row; };
		const function<int(int, int)> gPartCol = [](int col, int foundCol) { return col; };

		//CC
		const int C_rowStart = foundRow;
		const int C_colStart = foundCol;
		const int C_rowSize = foundRow + mapPiece.size();
		const int C_colSize = foundCol + mapPiece.front().size();
		compareCenter(gridMap, mapPiece, foundRow, foundCol, C_rowStart, C_colStart, C_rowSize, C_colSize, pPartRow, pPartCol, gPartRow, gPartCol, gridSearchLoopCount);
	}
	//2C
	else if (foundRow >= 0, foundCol > 0 &&
		foundRow + mapPiece.size() <= gridMap.size() &&
		foundCol + mapPiece.front().size() > gridMap.front().size())
	{
		cout << " 2C ";
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
		cout << " 3A ";
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
		deque<deque<MapElement>> Bottom_mapBlock = addToBottom(gridMap, mapPiece, foundRow, foundCol, Bottom_rowStart, Bottom_colStart, Bottom_rowSize, Bottom_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

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
		cout << " 3B ";
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
		deque<deque<MapElement>> Bottom_mapBlock = addToBottom(gridMap, mapPiece, foundRow, foundCol, Bottom_rowStart, Bottom_colStart, Bottom_rowSize, Bottom_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

		//Bottom +
		for (int i = 0; i < Bottom_mapBlock.size(); i++)
			gridMap.push_back(Bottom_mapBlock.at(i));
	}
	//3C
	else if (foundRow > 0 && foundCol > 0 &&
		foundCol + mapPiece.front().size() > gridMap.front().size() &&
		foundRow + mapPiece.size() > gridMap.size())
	{
		cout << " 3C " << endl;
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
		deque<deque<MapElement>> Bottom_mapBlock = addToBottom(gridMap, mapPiece, foundRow, foundCol, Bottom_rowStart, Bottom_colStart, Bottom_rowSize, Bottom_colSize, pPartRow, pPartCol, gPartRow, gPartCol, pPartCod, gridSearchLoopCount);

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
	cout << "Grid Search Loops: " << gridSearchLoopCount << " @ " << fixed << double((clock() - startGridLook) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";
}

void Thought::drawMap(deque<deque<MapElement>>& map, string windowName)
{
	Mat mapImg = Mat::zeros(map.size(), map.front().size(), CV_8UC3);
	for (int row = 0; row < map.size(); row++)
		for (int col = 0; col < map.front().size(); col++)
		{
			mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col).color;
		}
	//cv::resize(mapImg, mapImg, cv::Size(), 6, 6, INTER_NEAREST);
	imshow(windowName, mapImg);
};

void Thought::compareCenter(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int C_rowStart, const int C_colStart, const int C_rowSize, const int C_colSize, const function<int(int, int)>& pPartRow_CC, const function<int(int, int)>& pPartCol_CC, const function<int(int, int)>& gPartRow_CC, const function<int(int, int)>& gPartCol_CC, int& gridSearchLoopCount)
{
	for (int row = C_rowStart; row < C_rowSize; row++)
	{
		for (int col = C_colStart; col < C_colSize; col++)
		{
			gridSearchLoopCount++;
			if (mapPiece.at(pPartRow_CC(row, foundRow)).at(pPartCol_CC(col, foundCol)).type == 0)
				gridMap.at(gPartRow_CC(row, foundRow)).at(gPartCol_CC(col, foundCol)) = (mapPiece.at(pPartRow_CC(row, foundRow)).at(pPartCol_CC(col, foundCol)));
		}
	}
}

deque<deque<MapElement>> Thought::addToTop(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Top_rowStart, const int Top_colStart, const int Top_rowSize, const int Top_colSize, const function<int(int, int)>& pPartRow_Top, const function<int(int, int)>& pPartCol_Top, const function<int(int, int)>& gPartRow_Top, const function<int(int, int)>& gPartCol_Top, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Top, int& gridSearchLoopCount)
{
	deque<deque<MapElement>> Top_mapBlock;
	for (int row = Top_rowStart; row < Top_rowSize; row++)
	{
		deque<MapElement> mapLine;
		for (int col = Top_colStart; col < Top_colSize; col++)
		{
			gridSearchLoopCount++;

			if (pPartCod_Top(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Top(row, foundRow)).at(pPartCol_Top(col, foundCol)).type == 0)
					mapLine.push_back(mapPiece.at(pPartRow_Top(row, foundRow)).at(pPartCol_Top(col, foundCol)));
				else
					mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
			}
			else
				mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
		}
		Top_mapBlock.push_front(mapLine);
	}

	return Top_mapBlock;
}

deque<deque<MapElement>> Thought::addToBottom(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Bottom_rowStart, const int Bottom_colStart, const int Bottom_rowSize, const int Bottom_colSize, const function<int(int, int)>& pPartRow_Bottom, const function<int(int, int)>& pPartCol_Bottom, const function<int(int, int)>& gPartRow_Bottom, const function<int(int, int)>& gPartCol_Bottom, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Bottom, int& gridSearchLoopCount)
{
	deque<deque<MapElement>> Bottom_mapBlock;
	for (int row = Bottom_rowStart; row < Bottom_rowSize; row++)
	{
		deque<MapElement> mapLine;
		for (int col = Bottom_colStart; col < Bottom_colSize; col++)
		{
			gridSearchLoopCount++;

			if (pPartCod_Bottom(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Bottom(row, foundRow)).at(pPartCol_Bottom(col, foundCol)).type == 0)
					mapLine.push_back(mapPiece.at(pPartRow_Bottom(row, foundRow)).at(pPartCol_Bottom(col, foundCol)));
				else
					mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
			}
			else
				mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
		}
		if (foundRow < 0)
			Bottom_mapBlock.push_front(mapLine);
		else
			Bottom_mapBlock.push_back(mapLine);
	}

	return Bottom_mapBlock;
}

void Thought::addToLeft(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Left_rowStart, const int Left_colStart, const int Left_rowSize, const int Left_colSize, const function<int(int, int)>& pPartRow_Left, const function<int(int, int)>& pPartCol_Left, const function<int(int, int)>& gPartRow_Left, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Left, int& gridSearchLoopCount)
{
	for (int row = Left_rowStart; row < Left_rowSize; row++)
	{
		for (int col = Left_colSize - 1; col > Left_colStart; col--)
		{
			gridSearchLoopCount++;
			MapElement* mapElementToAppend;
			if (pPartCod_Left(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Left(row, foundRow)).at(pPartCol_Left(col, foundCol)).type == 0)
					mapElementToAppend = &(mapPiece.at(pPartRow_Left(row, foundRow)).at(pPartCol_Left(col, foundCol)));
				else
					mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
			}
			else
				mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

			gridMap.at(gPartRow_Left(row, foundRow)).push_front(*mapElementToAppend);
		}
	}
}

void Thought::addToRight(deque<deque<MapElement>>& gridMap, deque<deque<MapElement>>& mapPiece, int foundRow, int foundCol, const int Right_rowStart, const int Right_colStart, const int Right_rowSize, const int Right_colSize, const function<int(int, int)>& pPartRow_Right, const function<int(int, int)>& pPartCol_Right, const function<int(int, int)>& gPartRow_Right, const function<bool(int, int, int, int, const int, const int)>& pPartCod_Right, int& gridSearchLoopCount)
{
	for (int row = Right_rowStart; row < Right_rowSize; row++)
	{
		for (int col = Right_colStart; col < Right_colSize; col++)
		{
			gridSearchLoopCount++;
			MapElement* mapElementToAppend;
			if (pPartCod_Right(row, foundRow, col, foundCol, mapPiece.size(), mapPiece.front().size()))
			{
				if (mapPiece.at(pPartRow_Right(row, foundRow)).at(pPartCol_Right(col, foundCol)).type == 0)
					mapElementToAppend = &(mapPiece.at(pPartRow_Right(row, foundRow)).at(pPartCol_Right(col, foundCol)));
				else
					mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
			}
			else
				mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

			gridMap.at(gPartRow_Right(row, foundRow)).push_back(*mapElementToAppend);
		}
	}
}