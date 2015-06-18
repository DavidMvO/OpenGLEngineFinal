#include "CheckersBoard.h"
#include <iostream>

CheckersBoard::CheckersBoard()
{
	tilewidth = 10;
	tileheight = 10;

	kingHeight = 0;

	hasTilesSelected = false;

	//j is y rows, i is x columns
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			checkerBoard[i][j].selected = false;
			checkerBoard[i][j].available = false;
			checkerBoard[i][j].position = glm::vec3(0 + tilewidth*i, 0, 0 + tileheight*j);
			checkerBoard[i][j].boardPosition = glm::vec2(i, j);
			//for even y rows
			//if x is odd make black
			if ((j % 2) == 0 && (i % 2) != 0)
			{
				checkerBoard[i][j].colour = glm::vec4(0.15, 0.15, 0.15, 1);
				checkerBoard[i][j].originalColour = glm::vec4(0.15, 0.15, 0.15, 1);
			}
			//for even y rows
			//if x is even make red
			if ((j % 2) == 0 && (i % 2) == 0)
			{
				checkerBoard[i][j].colour = glm::vec4(1, 0.25, 0.25, 1);
				checkerBoard[i][j].originalColour = glm::vec4(1, 0.25, 0.25, 1);
				if (redPieces.size() < 12)
				{
					redPieces.push_back(CheckerPiece(GetBoardWorldPosition(i, j), glm::vec4(1, 0, 0, 1), false, glm::vec2(i, j)));
				}
				if (j >= 5)
				{
					if (blackPieces.size() < 12)
					{
						blackPieces.push_back(CheckerPiece(GetBoardWorldPosition(i, j), glm::vec4(0, 0, 0, 1), false, glm::vec2(i, j)));
					}
				}
			}

			//for odd y rows
			//if x is even make red
			if ((j % 2) != 0 && (i % 2) != 0)
			{
				checkerBoard[i][j].colour = glm::vec4(1, 0.25, 0.25, 1);
				checkerBoard[i][j].originalColour = glm::vec4(1, 0.25, 0.25, 1);
				if (redPieces.size() < 12)
				{
					redPieces.push_back(CheckerPiece(GetBoardWorldPosition(i, j), glm::vec4(1, 0, 0, 1), false, glm::vec2(i, j)));
				}
				if (j >= 5)
				{
					if (blackPieces.size() < 12)
					{
						blackPieces.push_back(CheckerPiece(GetBoardWorldPosition(i, j), glm::vec4(0, 0, 0, 1), false, glm::vec2(i, j)));
					}
				}
			}
			//for odd y rows
			//if x is even make black
			if ((j % 2) != 0 && (i % 2) == 0)
			{
				checkerBoard[i][j].colour = glm::vec4(0.15, 0.15, 0.15, 1);
				checkerBoard[i][j].originalColour = glm::vec4(0.15, 0.15, 0.15, 1);
			}
		}
	}
}

CheckersBoard::~CheckersBoard()
{

}

void CheckersBoard::Update(double dt)
{

}

void CheckersBoard::UpdateBoard()
{
	//j is y rows, i is x columns
	//fill board with gizmos at correct location
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			//if (checkerBoard[i][j].selected == true)
			//	checkerBoard[i][j].colour = glm::vec4(1, 1, 1, 1);

			//if (checkerBoard[i][j].available == true)
			//	checkerBoard[i][j].colour = glm::vec4(0, 1, 0, 1);
			if (hasTilesSelected)
				Gizmos::addAABBFilled(checkerBoard[i][j].position, glm::vec3(tilewidth/2, 1, tileheight/2), checkerBoard[i][j].colour);
			else if (!hasTilesSelected)
				Gizmos::addAABBFilled(checkerBoard[i][j].position, glm::vec3(tilewidth / 2, 1, tileheight / 2), checkerBoard[i][j].originalColour);
		}
	}

	for (int i = 0; i < redPieces.size(); i++)
	{
		if (redPieces[i].isKing == true)
			kingHeight = 2;
		else
			kingHeight = 0;

		Gizmos::addCylinderFilled(redPieces[i].position, 4, 2 + kingHeight, 20, redPieces[i].colour);

		//check for kings
		if (redPieces[i].boardPosition.y == 7)
			redPieces[i].isKing = true;
	}

	for (int i = 0; i < blackPieces.size(); i++)
	{
		if (blackPieces[i].isKing == true)
			kingHeight = 2;
		else
			kingHeight = 0;

		Gizmos::addCylinderFilled(blackPieces[i].position, 4, 2 + kingHeight, 20, blackPieces[i].colour);

		//check for kings
		if (blackPieces[i].boardPosition.y == 0)
			blackPieces[i].isKing = true;
	}

}

void CheckersBoard::Render()
{

}

glm::vec3 CheckersBoard::GetBoardWorldPosition(int row, int column)
{
	return checkerBoard[row][column].position;
}