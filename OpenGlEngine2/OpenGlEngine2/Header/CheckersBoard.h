#ifndef CHECKERS_BOARD_H
#define CHECKERS_BOARD_H

#include "Application.h"
#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <list>

struct BoardPiece
{
	glm::vec4 originalColour;
	glm::vec3 position;
	glm::vec4 colour;
	glm::vec2 boardPosition;
	bool selected;
	bool available;

};

struct CheckerPiece
{
	CheckerPiece(glm::vec3 piecePostion, glm::vec4 pieceColour, bool pieceIsKing, glm::vec2 pieceBoardPosition)
	{ position = piecePostion, colour = pieceColour, isKing = pieceIsKing; boardPosition = pieceBoardPosition; }

	glm::vec3 position;
	glm::vec2 boardPosition;
	glm::vec4 colour;
	bool isKing;
};

class CheckersBoard
{
public:

	CheckersBoard();
	~CheckersBoard();

	void Update(double dt);
	void Render();

	void UpdateBoard();
	void UpdatePieces();

	glm::vec3 GetBoardWorldPosition(int row, int column);


	BoardPiece checkerBoard[8][8];
	int tilewidth;
	int tileheight;

	std::vector<CheckerPiece> redPieces;
	std::vector<CheckerPiece> blackPieces;

	std::vector<BoardPiece*> availableMoves;

	int kingHeight;

	bool hasTilesSelected;

private:

};

#endif