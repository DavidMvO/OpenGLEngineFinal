#pragma once

#include <vector>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <list>

struct Move
{
	bool capture = nullptr;
	int score = NULL;
	glm::vec2 startingPieceLocation = glm::vec2(0,0);
	glm::vec2 capturePieceLocation = glm::vec2(0,0);
	glm::vec3 endingPieceLocation = glm::vec3(0,0,0);
	glm::vec2 endingPieceBoardLocation = glm::vec2(0, 0);
};

struct CaptureMove
{
	glm::vec2 CapturedPieceLocation;
	glm::vec3 CaptureMoveLocation;
};

class AI
{
public:

	enum Piece
	{
		BLACK,
		RED,
		NONE,
		OUTOFBOARD,
	};

	AI(CheckersBoard* board, int PlayOuts, int SearchDepth) { m_board = board; playouts = PlayOuts; searchDepth = SearchDepth; };
	~AI() {};

	//given the item to access from a list it moves the piece and removes any captures etc
	void MakeMove(std::vector<Move*> Moves, int MoveToMake)
	{
		for (int p = 0; p < m_board->blackPieces.size(); p++)
		{
			if (m_board->blackPieces[p].boardPosition == Moves[MoveToMake]->startingPieceLocation)
			{
				//move piece to currentTile(green)
				m_board->blackPieces[p].boardPosition = Moves[MoveToMake]->endingPieceBoardLocation;
				m_board->blackPieces[p].position = Moves[MoveToMake]->endingPieceLocation;

				if (Moves[MoveToMake]->capture == true)
				{
					for (int j = 0; j < m_board->redPieces.size(); j++)
					{
						if (m_board->redPieces[j].boardPosition == Moves[MoveToMake]->capturePieceLocation)
						{
							m_board->redPieces.erase(m_board->redPieces.begin() + j);
						}
					}
				}
			}
		}
		availableMoves.clear();
		if (mandatoryMoves.size() > 0)
			mandatoryMoves.clear();
		if (nonMandatoryMoves.size() > 0)
			nonMandatoryMoves.clear();
	}

	void GetValidMovesForBlack()
	{
		std::srand(std::time(0));
		//loop through all pieces
		//for each piece get available moves
		//add each move to list
		for (int row = 0; row < 8; row++)
		{
			for (int column = 0; column < 8; column++)
			{
				if (TileHasOpponentsPiece(column, row))
				{
					//---if piece is a king
					for (int i = 0; i < m_board->blackPieces.size(); i++)
					{
						if (m_board->blackPieces[i].boardPosition == glm::vec2(column, row))
						{
							if (m_board->blackPieces[i].isKing == true)
							{
								if (DownRight(column, row) == RED)
								{
									if (DownRight(column + 1, row + 1) == NONE)
									{
										Move* n = new Move();
										availableMoves.push_back(n);
										availableMoves.back()->capture = true;
										availableMoves.back()->score = 1;
										availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
										availableMoves.back()->capturePieceLocation = glm::vec2(column + 1, row + 1);
										availableMoves.back()->endingPieceLocation = glm::vec3((column + 2) * 10, 0, (row + 2) * 10);
										availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 2), (row + 2));
									}
								}
								if (DownLeft(column, row) == RED)
								{
									if (DownLeft(column - 1, row + 1) == NONE)
									{
										Move* n = new Move();
										availableMoves.push_back(n);
										availableMoves.back()->capture = true;
										availableMoves.back()->score = 1;
										availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
										availableMoves.back()->capturePieceLocation = glm::vec2(column - 1, row + 1);
										availableMoves.back()->endingPieceLocation = glm::vec3((column - 2) * 10, 0, (row + 2) * 10);
										availableMoves.back()->endingPieceBoardLocation = glm::vec2((column - 2), (row + 2));
									}
								}
								if (DownRight(column, row) == NONE)
								{
									Move* n = new Move();
									availableMoves.push_back(n);
									availableMoves.back()->capture = false;
									availableMoves.back()->score = 0;
									availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
									availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row + 1) * 10);
									availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row + 1));
								}
								if (DownLeft(column, row) == NONE)
								{
									Move* n = new Move();
									availableMoves.push_back(n);
									availableMoves.back()->capture = false;
									availableMoves.back()->score = 0;
									availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
									availableMoves.back()->endingPieceLocation = glm::vec3((column - 1) * 10, 0, (row + 1) * 10);
									availableMoves.back()->endingPieceBoardLocation = glm::vec2((column - 1), (row + 1));
								}
							}
						}
					}
					//-----------then default checks for black, non kings and kings
					if (UpRight(column, row) == RED)
					{
						if (UpRight(column + 1, row - 1) == NONE)
						{
							Move* n = new Move();
							availableMoves.push_back(n);
							availableMoves.back()->capture = true;
							availableMoves.back()->score = 1;
							availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
							availableMoves.back()->capturePieceLocation = glm::vec2(column + 1, row - 1);
							availableMoves.back()->endingPieceLocation = glm::vec3((column + 2) * 10, 0, (row - 2) * 10);
							availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 2), (row - 2));
						}
					}
					if (UpLeft(column, row) == RED)
					{
						if (UpLeft(column - 1, row - 1) == NONE)
						{
							Move* n = new Move();
							availableMoves.push_back(n);
							availableMoves.back()->capture = true;
							availableMoves.back()->score = 1;
							availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
							availableMoves.back()->capturePieceLocation = glm::vec2(column - 1, row - 1);
							availableMoves.back()->endingPieceLocation = glm::vec3((column - 2) * 10, 0, (row - 2) * 10);
							availableMoves.back()->endingPieceBoardLocation = glm::vec2((column - 2), (row - 2));
						}
					}
					if (UpRight(column, row) == NONE)
					{
						Move* n = new Move();
						availableMoves.push_back(n);
						availableMoves.back()->capture = false;
						availableMoves.back()->score = 0;
						availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
						availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row - 1) * 10);
						availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row - 1));
					}
					if (UpLeft(column, row) == NONE)
					{
						Move* n = new Move();
						availableMoves.push_back(n);
						availableMoves.back()->capture = false;
						availableMoves.back()->score = 0;
						availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
						availableMoves.back()->endingPieceLocation = glm::vec3((column - 1) * 10, 0, (row - 1) * 10);
						availableMoves.back()->endingPieceBoardLocation = glm::vec2((column -1), (row - 1));
					}
				}
			}
		}
		CalculatePotenitalMoves();
	}
	void GetValidMovesForRed()
	{


	}

	//out of all available moves, sort by priority and mandatory moves
	void CalculatePotenitalMoves()
	{
		//loop through available moves
		//check for captures, captures must be in mandatory list
		//if no mandatory moves
		//pick moves = to playouts for simulation
		if (availableMoves.size() != 0)
		{
			for (int i = 0; i < availableMoves.size(); i++)
			{
				if (availableMoves[i]->capture == true)
					mandatoryMoves.push_back(availableMoves[i]);
			}
			if (mandatoryMoves.size() > 0)
			{
				FindBestMove(mandatoryMoves);
			}
			else
			{
				//choose as many moves from availablemoves as there are playouts to add to the simulation
				while (nonMandatoryMoves.size() < playouts)
				{
					int r = rand() % availableMoves.size();
					nonMandatoryMoves.push_back(availableMoves[r]);
				}
				FindBestMove(nonMandatoryMoves);
			}
		}
	}

	void FindBestMove(std::vector<Move*> movesToCalculate)
	{
		//random call for testing
			//int r = rand() % movesToCalculate.size();
			//MakeMove(movesToCalculate, r);

		int playoutsRemaining = playouts;
		if (movesToCalculate.size() > 1)
		{
			while (playoutsRemaining > 0)
			{
				for (int i = 0; i < movesToCalculate.size(); i++)
				{
					//make a list of Move* called potential moves, put each of moves to calculate in that
					//for each potential move, pick 4 random available moves, for each of these moves, add the score to the potential move score
					//simulate a red move, deduct the red moves score from the potential move score
					//simulate black and red till playouts reached
					//find highest score in the potential move list
					//feed the move to MakeMove fucntion
				}


				playoutsRemaining - 1;
			}
		}
		else
			MakeMove(movesToCalculate, 0);
	}

	void CloneGame(std::vector<CheckerPiece> currentRedPieces, std::vector<CheckerPiece> currentBlackPieces)
	{
		currentRedPieces = redPieceSimulation;
		currentBlackPieces = blackPieceSimulation;
	}

	//v>
	Piece DownRight(int column, int row)
	{
		if (column + 1 > 7 || row + 1 > 7)
			return OUTOFBOARD;
		if (TileHasPlayersPiece(column + 1, row + 1))
			return RED;
		if (TileHasOpponentsPiece(column + 1, row + 1))
			return BLACK;
		if (!TileHasPlayersPiece(column + 1, row + 1) && !TileHasOpponentsPiece(column + 1, row + 1))
			return NONE;
	}
	//v<
	Piece DownLeft(int column, int row)
	{
		if (column - 1 < 0 || row + 1 > 7)
			return OUTOFBOARD;
		if (TileHasPlayersPiece(column - 1, row + 1))
			return RED;
		if (TileHasOpponentsPiece(column - 1, row + 1))
			return BLACK;
		if (!TileHasPlayersPiece(column - 1, row + 1) && !TileHasOpponentsPiece(column - 1, row + 1))
			return NONE;
	}
	//^>
	Piece UpRight(int column, int row)
	{
		if (column + 1 > 7 || row - 1 < 0)
			return OUTOFBOARD;
		if (TileHasPlayersPiece(column + 1, row - 1))
			return RED;
		if (TileHasOpponentsPiece(column + 1, row - 1))
			return BLACK;
		if (!TileHasPlayersPiece(column + 1, row - 1) && !TileHasOpponentsPiece(column + 1, row - 1))
			return NONE;
	}
	//^<
	Piece UpLeft(int column, int row)
	{
		if (column -1 < 0 || row -1 < 0)
			return OUTOFBOARD;
		if (TileHasPlayersPiece(column - 1, row - 1))
			return RED;
		if (TileHasOpponentsPiece(column - 1, row - 1))
			return BLACK;
		if (!TileHasPlayersPiece(column - 1, row - 1) && !TileHasOpponentsPiece(column - 1, row - 1))
			return NONE;
	}
	bool TileHasPlayersPiece(int column, int row)
	{
		// test player one
		for (int i = 0; i < m_board->redPieces.size(); i++)
		{
			if (m_board->redPieces[i].boardPosition.x == column &&
				m_board->redPieces[i].boardPosition.y == row)
				return true;
		}
		return false;
	}
	bool TileHasOpponentsPiece(int column, int row)
	{
		for (int i = 0; i < m_board->blackPieces.size(); i++)
		{
			if (m_board->blackPieces[i].boardPosition.x == column &&
				m_board->blackPieces[i].boardPosition.y == row)
				return true;
		}
		return false;
	}

	std::vector <CaptureMove> possibleCaptures;

	std::vector<Move*> availableMoves;
	std::vector<Move*> mandatoryMoves;
	std::vector<Move*> nonMandatoryMoves;
	std::vector<Move*> potentialMoves;

private:

	CheckersBoard* m_board;

	std::vector<CheckerPiece> redPieceSimulation;
	std::vector<CheckerPiece> blackPieceSimulation;
	BoardPiece SimulationCheckerBoard[8][8];

	int playouts;
	int searchDepth;
};