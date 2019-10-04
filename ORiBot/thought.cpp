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
								if (negativeMatch > 4)
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
						cout << " Merge Point: " << "(" << foundRow << "," << foundCol << ") ";

						//1A
						if (foundRow < 0 && foundCol < 0)
						{
							cout << " 1A ";
							//CC
							const int C_rowStart = -foundRow;
							const int C_colStart = -foundCol;
							const int C_rowSize = mapPiece.size();
							const int C_colSize = mapPiece.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}
							
							//Top
							const int Top_rowStart = 0;
							const int Top_colStart = 0;
							const int Top_rowSize = -foundRow;
							const int Top_colSize = gridMap.front().size() - foundCol;
							deque<deque<MapElement>> Top_mapBlock;
							for (int row = Top_rowStart; row < Top_rowSize; row++)
							{
								deque<MapElement> mapLine;
								const int pPartRow = row;
								for (int col = Top_colStart; col < Top_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const bool pPartCod = (row < mapPiece.size() && col < mapPiece.front().size());

									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapLine.push_back(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
								}
									Top_mapBlock.push_front(mapLine);
							}

							//Left
							const int Left_rowStart = -foundRow;
							const int Left_colStart = 0;
							const int Left_rowSize = -foundRow + gridMap.size();
							const int Left_colSize = -foundCol +1;
							for (int row = Left_rowStart; row < Left_rowSize; row++)
							{
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = Left_colSize - 1; col > Left_colStart; col--)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									const bool pPartCod = (row < mapPiece.size() && col < mapPiece.front().size());

									MapElement* mapElementToAppend;
									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapElementToAppend = &(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

										gridMap.at(gPartRow).push_front(*mapElementToAppend);
								}
							}

							//Top +
							for (int i = 0; i < Top_mapBlock.size(); i++)
								gridMap.push_front(Top_mapBlock.at(i));
						}
						//1B
						else if (foundRow < 0 && foundCol >= 0 &&
							foundCol + mapPiece.front().size() <= gridMap.front().size())
						{
							cout << " 1B ";
							//CC
							const int C_rowStart = -foundRow;
							const int C_colStart = foundCol;
							const int C_rowSize = mapPiece.size();
							const int C_colSize = foundCol + mapPiece.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}

							//Top
							const int Top_rowStart = 0;
							const int Top_colStart = 0;
							const int Top_rowSize = -foundRow;
							const int Top_colSize = gridMap.front().size();
							deque<deque<MapElement>> Top_mapBlock;
							for (int row = Top_rowStart; row < Top_rowSize; row++)
							{
								deque<MapElement> mapLine;
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = Top_colStart; col < Top_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool pPartCod = (row < -foundRow && col >= foundCol && col < foundCol + mapPiece.front().size());

									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapLine.push_back(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
								}
									Top_mapBlock.push_front(mapLine);
							}

							//Top +
							for (int i = 0; i < Top_mapBlock.size(); i++)
								gridMap.push_front(Top_mapBlock.at(i));
						}
						//1C
						else if (foundRow < 0 && foundCol > 0 &&
							foundCol + mapPiece.front().size() >= gridMap.front().size())
						{
							cout << " 1C ";
							//CC
							const int C_rowStart = -foundRow;
							const int C_colStart = foundCol;
							const int C_rowSize = mapPiece.size();
							const int C_colSize = gridMap.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}
							//Right
							const int Right_rowStart = -foundRow;
							const int Right_colStart = gridMap.front().size();
							const int Right_rowSize = -foundRow + gridMap.size();
							const int Right_colSize = foundCol + mapPiece.front().size();
							for (int row = Right_rowStart; row < Right_rowSize; row++)
							{
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = Right_colStart; col < Right_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool pPartCod = (row < mapPiece.size() && col >= foundCol);

									MapElement* mapElementToAppend;
									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapElementToAppend = &(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

										gridMap.at(gPartRow).push_back(*mapElementToAppend);
								}
							}

							//Top
							const int Top_rowStart = 0;
							const int Top_colStart = 0;
							const int Top_rowSize = -foundRow;
							const int Top_colSize = mapPiece.front().size() + foundCol;
							deque<deque<MapElement>> Top_mapBlock;
							for (int row = Top_rowStart; row < Top_rowSize; row++)
							{
								deque<MapElement> mapLine;
								const int pPartRow = row;
								const int gPartRow = row + foundRow;
								for (int col = Top_colStart; col < Top_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool pPartCod = (row < mapPiece.size() && col >= foundCol);

									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapLine.push_back(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapLine.push_back(MapElement("Unseen", Vec3b(0, 255, 255), 2));
								}
									Top_mapBlock.push_front(mapLine);
							}

							//Top +
							for (int i = 0; i < Top_mapBlock.size(); i++)
								gridMap.push_front(Top_mapBlock.at(i));							
						}
						//2A
						else if (foundRow >= 0 && foundCol < 0 &&
							foundRow + mapPiece.size() <= gridMap.size())
						{
							cout << " 2A ";
							//CC
							const int C_rowStart = foundRow;
							const int C_colStart = -foundCol;
							const int C_rowSize = foundRow + mapPiece.size();
							const int C_colSize = mapPiece.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}
							//Left
							const int Left_rowStart = 0;
							const int Left_colStart = 0;
							const int Left_rowSize = gridMap.size();
							const int Left_colSize = -foundCol;
							for (int row = Left_rowStart; row < Left_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = Left_colSize - 1; col > Left_colStart; col--)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									const bool pPartCod = (row >= foundRow && col < mapPiece.front().size() && row < foundRow + mapPiece.size());

									MapElement* mapElementToAppend;
									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapElementToAppend = &(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

										gridMap.at(gPartRow).push_front(*mapElementToAppend);
								}
							}
						}
						//2B
						else if (foundRow >= 0 && foundCol >= 0 &&
							foundRow + mapPiece.size() <= gridMap.size() &&
							foundCol + mapPiece.front().size() <= gridMap.front().size())
						{
							cout << " 2B ";
							//CC
							const int C_rowStart = foundRow;
							const int C_colStart = foundCol;
							const int C_rowSize = foundRow + mapPiece.size();
							const int C_colSize = foundCol + mapPiece.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}
						}
						//2C
						else if (foundRow >= 0, foundCol > 0 &&
							foundRow + mapPiece.size() <= gridMap.size() &&
							foundCol + mapPiece.front().size() > gridMap.front().size())
						{
							cout << " 2C ";
							//CC
							const int C_rowStart = foundRow;
							const int C_colStart = foundCol;
							const int C_rowSize = foundRow + mapPiece.size();
							const int C_colSize = gridMap.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}
							//Right
							const int Right_rowStart = 0;
							const int Right_colStart = gridMap.front().size();
							const int Right_rowSize = gridMap.size();
							const int Right_colSize = foundCol + mapPiece.front().size();
							for (int row = Right_rowStart; row < Right_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = Right_colStart; col < Right_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool pPartCod = (row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size());

									MapElement* mapElementToAppend;
									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapElementToAppend = &(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

										gridMap.at(gPartRow).push_back(*mapElementToAppend);
								}
							}
						}
						//3A
						else if (foundRow > 0 && foundCol < 0 &&
							foundRow + mapPiece.size() > gridMap.size())
						{
							cout << " 3A ";
							//CC
							const int C_rowStart = foundRow;
							const int C_colStart = -foundCol;
							const int C_rowSize = gridMap.size();
							const int C_colSize = mapPiece.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}

							//Bottom
							const int Bottom_rowStart = gridMap.size();
							const int Bottom_colStart = 0;
							const int Bottom_rowSize = foundRow + mapPiece.size();
							const int Bottom_colSize = -foundCol + gridMap.front().size();
							deque<deque<MapElement>> Bottom_mapBlock;
							for (int row = Bottom_rowStart; row < Bottom_rowSize; row++)
							{
								deque<MapElement> mapLine;
								const int pPartRow = row - foundRow;
								for (int col = Bottom_colStart; col < Bottom_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const bool pPartCod = (row >= foundRow && col < mapPiece.front().size());

									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapLine.push_back(mapPiece.at(pPartRow).at(pPartCol));
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

							//Left
							const int Left_rowStart = 0;
							const int Left_colStart = 0;
							const int Left_rowSize = gridMap.size();
							const int Left_colSize = -foundCol + 1;
							for (int row = Left_rowStart; row < Left_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = Left_colSize - 1; col > Left_colStart; col--)
								{
									gridSearchLoopCount++;
									const int pPartCol = col;
									const int gPartCol = col + foundCol;
									const bool pPartCod = (row >= foundRow && col < mapPiece.front().size());

									MapElement* mapElementToAppend;
									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapElementToAppend = &(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

										gridMap.at(gPartRow).push_front(*mapElementToAppend);
								}
							}

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
							//CC
							const int C_rowStart = foundRow;
							const int C_colStart = foundCol;
							const int C_rowSize = gridMap.size();
							const int C_colSize = foundCol + mapPiece.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}

							//Bottom
							const int Bottom_rowStart = gridMap.size();
							const int Bottom_colStart = 0;
							const int Bottom_rowSize = foundRow + mapPiece.size();
							const int Bottom_colSize = gridMap.front().size();
							deque<deque<MapElement>> Bottom_mapBlock;
							for (int row = Bottom_rowStart; row < Bottom_rowSize; row++)
							{
								deque<MapElement> mapLine;
								const int pPartRow = row - foundRow;
								for (int col = Bottom_colStart; col < Bottom_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const bool pPartCod = (row >= foundRow && col >= foundCol && col < foundCol + mapPiece.front().size());

									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapLine.push_back(mapPiece.at(pPartRow).at(pPartCol));
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
							//CC
							const int C_rowStart = foundRow;
							const int C_colStart = foundCol;
							const int C_rowSize = gridMap.size();
							const int C_colSize = gridMap.front().size();
							for (int row = C_rowStart; row < C_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = C_colStart; col < C_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
										gridMap.at(gPartRow).at(gPartCol) = (mapPiece.at(pPartRow).at(pPartCol));
								}
							}

							//Bottom
							const int Bottom_rowStart = gridMap.size();
							const int Bottom_colStart = 0;
							const int Bottom_rowSize = foundRow + mapPiece.size();
							const int Bottom_colSize = foundCol + mapPiece.front().size();
							deque<deque<MapElement>> Bottom_mapBlock;
							for (int row = Bottom_rowStart; row < Bottom_rowSize; row++)
							{
								deque<MapElement> mapLine;
								const int pPartRow = row - foundRow;
								for (int col = Bottom_colStart; col < Bottom_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const bool pPartCod = (row >= foundRow && col >= foundCol);

									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapLine.push_back(mapPiece.at(pPartRow).at(pPartCol));
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

							//Right
							const int Right_rowStart = 0;
							const int Right_colStart = gridMap.front().size();
							const int Right_rowSize = gridMap.size();
							const int Right_colSize = foundCol + mapPiece.front().size();
							for (int row = Right_rowStart; row < Right_rowSize; row++)
							{
								const int pPartRow = row - foundRow;
								const int gPartRow = row;
								for (int col = Right_colStart; col < Right_colSize; col++)
								{
									gridSearchLoopCount++;
									const int pPartCol = col - foundCol;
									const int gPartCol = col;
									const bool pPartCod = (row >= foundRow && col >= foundCol);

									MapElement* mapElementToAppend;
									if (pPartCod)
									{
										if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
											mapElementToAppend = &(mapPiece.at(pPartRow).at(pPartCol));
										else
											mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));
									}
									else
										mapElementToAppend = (new MapElement("Unseen", Vec3b(0, 255, 255), 2));

									gridMap.at(gPartRow).push_back(*mapElementToAppend);
								}
							}
							//Bottom +
							for (int i = 0; i < Bottom_mapBlock.size(); i++)
								gridMap.push_back(Bottom_mapBlock.at(i));
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