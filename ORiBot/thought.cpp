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
	bool matchFound = false;
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
		int matches = 0;
		int negativeMatch = 0;

		int xStart, yStart, xEnd, yEnd;

		if (lastGridPos.x != -1 || lastGridPos.y != -1)
		{
			xStart = lastGridPos.x; if (xStart - border >= 0) xStart -= border;
			yStart = lastGridPos.y; if (yStart - border >= 0) yStart -= border;
			xEnd = xStart + border + mapPiece.size();
			yEnd = yStart + border + mapPiece.front().size();
		}
		else
		{
			xStart = 0;
			yStart = 0;
			xEnd = gridMap.size();
			yEnd = gridMap.front().size();
		}

		cout << "Start" << "(" << xStart << "," << yStart << ") ";// << endl;
		for (int foundRow = xStart; foundRow < gridMap.size() - (mapPiece.size() - border * 2) && foundRow < xEnd; foundRow++)
			for (int foundCol = yStart; foundCol < gridMap.front().size() - (mapPiece.front().size() - border * 2) && foundCol < yEnd; foundCol++)
			{
				if (gridMap.at(foundRow).at(foundCol).name == mapPiece.at(border).at(border).name || gridMap.at(foundRow).at(foundCol).type == 1 || mapPiece.at(border).at(border).type == 1)
				{
					matches = 1;
					negativeMatch = 0;
					for (int row = 1; ((row + foundRow) < gridMap.size() && row < (mapPiece.size() - border * 2)); row++)
					{
						for (int col = 1; ((col + foundCol) < gridMap.front().size() && col < (mapPiece.front().size() - border * 2)); col++)
						{
							matchSearchLoopCount++;
							if (gridMap.at((row + foundRow)).at((col + foundCol)).name == mapPiece.at(row + border).at(col + border).name ||
								(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableA" && mapPiece.at(row + border).at(col + border).name == "WalkableB") ||
								(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableB" && mapPiece.at(row + border).at(col + border).name == "WalkableA") ||
								gridMap.at((row + foundRow)).at((col + foundCol)).type == 1 ||
								mapPiece.at(row + border).at(col + border).type == 1)
							{
								matches++;
							}
							else
							{
								negativeMatch++;
								if (negativeMatch > 5)
									goto nextGrid;
							}
						}
					}
					matchFound = true;
				nextGrid:
					//Append to Grid Map
					if (matchFound)
					{
						int gridSearchLoopCount = 0;
						cout << "Match Search Loops: " << matchSearchLoopCount << " @ " << fixed << double((clock() - startMatch) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";
						clock_t startGridLook = clock();
						lastGridPos = Point(foundRow, foundCol);
						cout << "Found (" << foundRow << "," << foundCol << ") ";// << endl;
						foundRow -= border;
						foundCol -= border;
						//cout << "Merge Point: " << "(" << foundRow << "," << foundCol << ")";

						deque<deque<MapElement>> newGridMap;


						//1A
						if (foundRow < 0 && foundCol < 0)
						{
							//cout << " 1A" << endl;
							const int rowSize = -foundRow + gridMap.size();
							const int colSize = -foundCol + gridMap.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								deque<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row < mapPiece.size() && col < mapPiece.front().size());
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									const bool gPartCod = (row >= -foundRow && col >= -foundCol);
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//1B
						else if (foundRow < 0 && foundCol >= 0 &&
							foundCol + mapPiece.front().size() <= gridMap.front().size())
						{
							//cout << " 1B" << endl;
							const int rowSize = -foundRow + gridMap.size();
							const int colSize = gridMap.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								deque<MapElement> newGridMapLine;
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row < -foundRow && col >= foundCol && col < foundCol + mapPiece.front().size());
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool gPartCod = (row >= -foundRow);
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//1C
						else if (foundRow < 0 && foundCol > 0 &&
							foundCol + mapPiece.front().size() >= gridMap.front().size())
						{
							//cout << " 1C" << endl;
							const int rowSize = -foundRow + gridMap.size();
							const int colSize = foundCol + mapPiece.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								deque<MapElement> newGridMapLine;
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row < mapPiece.size() && col >= foundCol);
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool gPartCod = (row >= -foundRow && col < gridMap.at(gPartRow).size());
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//2A
						else if (foundRow >= 0 && foundCol < 0 &&
							foundRow + mapPiece.size() <= gridMap.size())
						{
							//cout << " 2A" << endl;
							const int rowSize = gridMap.size();
							const int colSize = -foundCol + gridMap.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								deque<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row >= foundRow && col < mapPiece.front().size() && row < foundRow + mapPiece.size());
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									const bool gPartCod = (col >= -foundCol);
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//2B
						else if (foundRow >= 0 && foundCol >= 0 &&
							foundRow + mapPiece.size() <= gridMap.size() &&
							foundCol + mapPiece.front().size() <= gridMap.front().size())
						{
							//cout << " 2B" << endl;
							const int rowSize = gridMap.size();
							const int colSize = gridMap.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								deque<MapElement> newGridMapLine;
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size() && col < foundCol + mapPiece.front().size());
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool gPartCod = (true);
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//2C
						else if (foundRow >= 0, foundCol > 0 &&
							foundRow + mapPiece.size() <= gridMap.size() &&
							foundCol + mapPiece.front().size() > gridMap.front().size())
						{
							//cout << " 2C" << endl;
							const int rowSize = gridMap.size();
							const int colSize = foundCol + mapPiece.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								deque<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size());
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool gPartCod = (col < gridMap.at(gPartRow).size());
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//3A
						else if (foundRow > 0 && foundCol < 0 &&
							foundRow + mapPiece.size() > gridMap.size())
						{
							//cout << " 3A" << endl;
							const int rowSize = foundRow + mapPiece.size();
							const int colSize = -foundCol + gridMap.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								deque<MapElement> newGridMapLine;
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row >= foundRow && col < mapPiece.front().size());
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									const bool gPartCod = (row < gridMap.size() && col >= -foundCol);
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//3B
						else if (foundRow > 0 && foundCol >= 0 &&
							foundCol + mapPiece.front().size() <= gridMap.front().size() &&
							foundRow + mapPiece.size() > gridMap.size())
						{
							//cout << " 3B " << endl;
							const int rowSize = foundRow + mapPiece.size();
							const int colSize = gridMap.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								deque<MapElement> newGridMapLine;
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row >= foundRow && col >= foundCol && col < foundCol + mapPiece.front().size());
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool gPartCod = (row < gridMap.size());
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						//3C
						else if (foundRow > 0 && foundCol > 0 &&
							foundCol + mapPiece.front().size() > gridMap.front().size() &&
							foundRow + mapPiece.size() > gridMap.size())
						{
							//cout << " 3C " << endl;
							const int rowSize = foundRow + mapPiece.size();
							const int colSize = foundCol + mapPiece.front().size();
							deque<deque<MapElement>> newGridMapBlock;
							for (int row = 0; row < rowSize; row++)
							{
								deque<MapElement> newGridMapLine;
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = 0; col < colSize; col++)
								{
									gridSearchLoopCount++;
									const bool pPartCod = (row >= foundRow && col >= foundCol);
									const int pPartCol = col - foundCol;
									const bool gPartCod = (row < gridMap.size() && col < gridMap.at(gPartRow).size());
									const int gPartCol = col;
									if ((gridMap.size() + newGridMapBlock.size() == rowSize && foundRow < 0) ||
										(gridMap.size() > gPartRow && foundRow >= 0))
									{
										if (gPartCod)
										{
											if (gridMap.at(gPartRow).at(gPartCol).type != 0)
												gridMap.at(gPartRow).at(gPartCol) = (innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
										}
										else
											newGridMapLine.push_back((innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap)));
									}
									else
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								if ((gridMap.size() + newGridMapBlock.size() < rowSize && foundRow < 0) ||
									(gridMap.size() <= gPartRow && foundRow >= 0))
								{
									if (foundRow < 0)
										newGridMapBlock.push_front(newGridMapLine);
									else
										newGridMapBlock.push_back(newGridMapLine);
								}
								else
									if (foundCol < 0)
										for (int i = newGridMapLine.size() - 1; i >= 0; i--)
											gridMap.at(gPartRow).push_front(newGridMapLine.at(i));
									else
										for (int i = 0; i < newGridMapLine.size(); i++)
											gridMap.at(gPartRow).push_back(newGridMapLine.at(i));
							}
							for (int i = 0; i < newGridMapBlock.size(); i++)
								if (foundRow < 0)
									gridMap.push_front(newGridMapBlock.at(i));
								else
									gridMap.push_back(newGridMapBlock.at(i));
						}
						drawMap(gridMap, "grid");
						cout << "Grid Search Loops: " << gridSearchLoopCount << " @ " << fixed << double((clock() - startGridLook) / double(CLOCKS_PER_SEC)) * 1000 << setprecision(0); cout << "ms  ";
						return;
					}
					else
						lastGridPos = Point(-1, -1);
				}
			}
	}
};

void Thought::pushToMap(int pushRow, int pushCol, int& foundRow, int& foundCol, MapElement mapElement)
{

}

MapElement Thought::innerConditions(const bool pPartCod, const bool gPartCod, const int pPartRow, const int pPartCol, const int gPartRow, const int gPartCol, deque<deque<MapElement>>& mapPiece, deque<deque<MapElement>>& gridMap)
{
	if (pPartCod && gPartCod) //CC
	{
		if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
			return (mapPiece.at(pPartRow).at(pPartCol));

		if (gridMap.at(gPartRow).at(gPartCol).type == 0)
			return (gridMap.at(gPartRow).at(gPartCol));


		return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	}

	if (pPartCod) //P
	{
		if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
			return (mapPiece.at(pPartRow).at(pPartCol));


		return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	}

	if (gPartCod) //G
	{
		if (gridMap.at(gPartRow).at(gPartCol).type == 0)
			return(gridMap.at(gPartRow).at(gPartCol));


		return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	}

	return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
};

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