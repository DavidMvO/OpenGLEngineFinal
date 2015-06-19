#pragma once

#include <vector>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <list>
#include <math.h>

#include <thread>
#include <mutex>

#include <iostream>
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
		UNDEFINE,
	};

	AI(CheckersBoard* board, int PlayOuts, int SearchDepth) 
	{ 
		m_board = board;
		playouts = PlayOuts; 
		playoutsTotal = PlayOuts;
		searchDepth = SearchDepth;
		SearchDepthTotal = SearchDepth;
		pieceCaptured = false; 
	};
	~AI() {};

	//given the item to access from a list it moves the piece and removes any captures etc
	void MakeMove(std::vector<Move*> Moves, int MoveToMake, std::vector<CheckerPiece>& PiecesToMove, std::vector<CheckerPiece>& PiecesToCapture, bool FinalMove)
	{
		for (int p = 0; p < PiecesToMove.size(); p++)
		{
			if (PiecesToMove[p].boardPosition == Moves[MoveToMake]->startingPieceLocation)
			{
				//move piece to currentTile(green)
				PiecesToMove[p].boardPosition = Moves[MoveToMake]->endingPieceBoardLocation;
				PiecesToMove[p].position = Moves[MoveToMake]->endingPieceLocation;

				if (Moves[MoveToMake]->capture == true)
				{
					for (int j = 0; j < PiecesToCapture.size(); j++)
					{
						if (PiecesToCapture[j].boardPosition == Moves[MoveToMake]->capturePieceLocation)
						{
							PiecesToCapture.erase(PiecesToCapture.begin() + j);
							if (FinalMove == true)
								pieceCaptured = true;
						}
					}
				}
				else
					pieceCaptured = false;
			}
		}
		availableMoves.clear();
		if (mandatoryMoves.size() > 0)
			mandatoryMoves.clear();
		if (nonMandatoryMoves.size() > 0)
			nonMandatoryMoves.clear();

		if (FinalMove == true && pieceCaptured == true)
		{
			bonusMove = true;
			ConsecutiveMove(Moves[MoveToMake]->endingPieceBoardLocation.x, Moves[MoveToMake]->endingPieceBoardLocation.y, m_board->redPieces, m_board->blackPieces);
			bonusMove = false;
		}
	}

	void GetValidMovesForBlack(std::vector<CheckerPiece>& PiecesToCheck, std::vector<CheckerPiece>& otherPieces)
	{
		std::srand(std::time(0));
		//loop through all pieces
		//for each piece get available moves
		//add each move to list
		for (int row = 0; row < 8; row++)
		{
			for (int column = 0; column < 8; column++)
			{
				if (TileHasOpponentsPiece(column, row, PiecesToCheck))
				{
					//---if piece is a king
					for (int i = 0; i < PiecesToCheck.size(); i++)
					{
						if (PiecesToCheck[i].boardPosition == glm::vec2(column, row))
						{
							if (PiecesToCheck[i].isKing == true)
							{
								if (DownRight(column, row, otherPieces, PiecesToCheck) == RED)
								{
									if (DownRight(column + 1, row + 1, otherPieces, PiecesToCheck) == NONE)
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
								if (DownLeft(column, row, otherPieces, PiecesToCheck) == RED)
								{
									if (DownLeft(column - 1, row + 1, otherPieces, PiecesToCheck) == NONE)
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
								if (DownRight(column, row, otherPieces, PiecesToCheck) == NONE)
								{
									Move* n = new Move();
									availableMoves.push_back(n);
									availableMoves.back()->capture = false;
									availableMoves.back()->score = 0;
									availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
									availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row + 1) * 10);
									availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row + 1));

								}
								if (DownLeft(column, row, otherPieces, PiecesToCheck) == NONE)
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
					if (UpRight(column, row, otherPieces, PiecesToCheck) == RED)
					{
						if (UpRight(column + 1, row - 1, otherPieces, PiecesToCheck) == NONE)
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
					if (UpLeft(column, row, otherPieces, PiecesToCheck) == RED)
					{
						if (UpLeft(column - 1, row - 1, otherPieces, PiecesToCheck) == NONE)
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
					if (UpRight(column, row, otherPieces, PiecesToCheck) == NONE)
					{
						Move* n = new Move();
						availableMoves.push_back(n);
						availableMoves.back()->capture = false;
						availableMoves.back()->score = 0;
						availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
						availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row - 1) * 10);
						availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row - 1));
					}
					if (UpLeft(column, row, otherPieces, PiecesToCheck) == NONE)
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
		for (int i = 0; i < availableMoves.size(); i++)
		{
			if (availableMoves[i]->capture == true)
				mandatoryMoves.push_back(availableMoves[i]);
		}
		if (mandatoryMoves.size() > 0)
			availableMoves = mandatoryMoves;
	}
	void SimulateRedTurn(std::vector<CheckerPiece>& PiecesToCheck, std::vector<CheckerPiece>& otherPieces)
	{
		//find all moves for red
		for (int row = 0; row < 8; row++)
		{
			for (int column = 0; column < 8; column++)
			{
				if (TileHasPlayersPiece(column, row, PiecesToCheck))
				{
					//---if piece is a king
					for (int i = 0; i < PiecesToCheck.size(); i++)
					{
						if (PiecesToCheck[i].boardPosition == glm::vec2(column, row))
						{
							if (PiecesToCheck[i].isKing == true)
							{
								if (UpRight(column, row, PiecesToCheck, otherPieces) == BLACK)
								{
									if (UpRight(column + 1, row - 1, PiecesToCheck, otherPieces) == NONE)
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
								if (UpLeft(column, row, PiecesToCheck, otherPieces) == BLACK)
								{
									if (UpLeft(column - 1, row - 1, PiecesToCheck, otherPieces) == NONE)
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
								if (UpRight(column, row, PiecesToCheck, otherPieces) == NONE)
								{
									Move* n = new Move();
									availableMoves.push_back(n);
									availableMoves.back()->capture = false;
									availableMoves.back()->score = 0;
									availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
									availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row - 1) * 10);
									availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row - 1));
								}
								if (UpLeft(column, row, PiecesToCheck, otherPieces) == NONE)
								{
									Move* n = new Move();
									availableMoves.push_back(n);
									availableMoves.back()->capture = false;
									availableMoves.back()->score = 0;
									availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
									availableMoves.back()->endingPieceLocation = glm::vec3((column - 1) * 10, 0, (row - 1) * 10);
									availableMoves.back()->endingPieceBoardLocation = glm::vec2((column - 1), (row - 1));
								}
							}
						}
					}
					//-----------then default checks for red, non kings and kings
					if (DownRight(column, row, PiecesToCheck, otherPieces) == BLACK)
					{
						if (DownRight(column + 1, row + 1, PiecesToCheck, otherPieces) == NONE)
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
					if (DownLeft(column, row, PiecesToCheck, otherPieces) == BLACK)
					{
						if (DownLeft(column - 1, row + 1, PiecesToCheck, otherPieces) == NONE)
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
					if (DownRight(column, row, PiecesToCheck, otherPieces) == NONE)
					{
						Move* n = new Move();
						availableMoves.push_back(n);
						availableMoves.back()->capture = false;
						availableMoves.back()->score = 0;
						availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
						availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row + 1) * 10);
						availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row + 1));
					}
					if (DownLeft(column, row, PiecesToCheck, otherPieces) == NONE)
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
		for (int i = 0; i < availableMoves.size(); i++)
		{
			if (availableMoves[i]->capture == true)
				mandatoryMoves.push_back(availableMoves[i]);
		}
		if (mandatoryMoves.size() > 0)
			availableMoves = mandatoryMoves;


		////find potential moves for red
		//if (availableMoves.size() != 0)
		//{
		//	for (int i = 0; i < availableMoves.size(); i++)
		//	{
		//		if (availableMoves[i]->capture == true)
		//			mandatoryMoves.push_back(availableMoves[i]);
		//	}
		//	if (mandatoryMoves.size() > 0)
		//	{
		//		//choose a random mandatory move
		//		int r = rand() % mandatoryMoves.size();
		//		moveToModify->score -= mandatoryMoves[r]->score*15;
		//		MakeMove(mandatoryMoves, r, redPieceSimulation, blackPieceSimulation, false);
		//	}
		//	else
		//	{
		//		//choose as many moves from availablemoves as there are playouts to add to the simulation
		//		while (nonMandatoryMoves.size() < playouts)
		//		{
		//			int r = rand() % availableMoves.size();
		//			nonMandatoryMoves.push_back(availableMoves[r]);
		//		}
		//		//choose a random non mandatory move
		//		int r = rand() % nonMandatoryMoves.size();
		//		moveToModify->score -= nonMandatoryMoves[r]->score*15;
		//		MakeMove(nonMandatoryMoves, r, redPieceSimulation, blackPieceSimulation, false);
		//	}
		//}
	}

	//out of all available moves, sort by priority and mandatory moves
	void CalculatePotentialMoves()
	{
		//get a list of available actions
		GetValidMovesForBlack(m_board->blackPieces, m_board->redPieces);
		//save moves
		potentialMoves = availableMoves;
		for (int i = 0; i < potentialMoves.size(); i++)
		{
			//for each potential move, set the score to 0
			potentialMoves[i]->score = 0;
			for (int j = 0; j < playouts; j++)
			{
				searchDepth = SearchDepthTotal;
				ThreadedSearch(i);
			}
		}
		//pick highest scoring move

		int highestScore = 0;
		bestMove = 0;
		for (int i = 0; i < potentialMoves.size()-1; i++)
		{
			if (potentialMoves[i]->score >= highestScore)
			{
				highestScore = potentialMoves[i]->score;
				bestMove = i;
			}
		}
		MakeMove(potentialMoves, bestMove, m_board->blackPieces, m_board->redPieces, true);
		potentialMoves.clear();
	}

	void ThreadedSearch(int potentialMoveID)
	{
		//for each playout clone the game
		CloneGame(m_board->redPieces, m_board->blackPieces);
		//loop till game ends
		while (blackPieceSimulation.size() > 0 && redPieceSimulation.size() > 0 && searchDepth > 0)
		{
			//as long as the game is not over
			//std::cout << availableMoves.size();
			//choose black turn
			if (availableMoves.size() == 0)
				GetValidMovesForBlack(blackPieceSimulation, redPieceSimulation);

			if (availableMoves.size() != 0)
			{
				int r = rand() % availableMoves.size();
				//add capture score
				if (availableMoves[r]->capture == true)
				{
					for (int o = 0; o < blackPieceSimulation.size(); o++)
					{
						if (availableMoves[r]->capturePieceLocation == blackPieceSimulation[o].boardPosition)
						{
							if (blackPieceSimulation[o].isKing == true)
								potentialMoves[potentialMoveID]->score += 15;
						}
					}
					potentialMoves[potentialMoveID]->score += 1;
				}

				MakeMove(availableMoves, r, blackPieceSimulation, redPieceSimulation, false);
			}
			//DisplayDebugArray();

			for (int b = 0; b < blackPieceSimulation.size(); b++)
			{
				if (blackPieceSimulation[b].boardPosition.y == 0)
					blackPieceSimulation[b].isKing = true;
			}
			if (redPieceSimulation.size() == 0)
			{
				break;
			}
			if (blackPieceSimulation.size() == 0)
			{
				break;
			}

			//make red turn
			SimulateRedTurn(redPieceSimulation, blackPieceSimulation);
			//std::cout << availableMoves.size();
			if (availableMoves.size() != 0)
			{
				int k = rand() % availableMoves.size();
				//deduct enemy capture score
				if (availableMoves[k]->capture == true)
				{
					for (int o = 0; o < blackPieceSimulation.size(); o++)
					{
						if (availableMoves[k]->capturePieceLocation == blackPieceSimulation[o].boardPosition)
						{
							if (blackPieceSimulation[o].isKing == true)
								potentialMoves[potentialMoveID]->score -= 20;
						}
					}
					potentialMoves[potentialMoveID]->score -= 2;
				}
				MakeMove(availableMoves, k, redPieceSimulation, blackPieceSimulation, false);
			}

			//DisplayDebugArray();

			for (int p = 0; p < redPieceSimulation.size(); p++)
			{
				if (redPieceSimulation[p].boardPosition.y == 7)
					redPieceSimulation[p].isKing = true;
			}

			//make black turn
			GetValidMovesForBlack(blackPieceSimulation, redPieceSimulation);
			searchDepth -= 1;
		}

		//add points for wins and deduct for losses
		if (blackPieceSimulation.size() <= 0)
			potentialMoves[potentialMoveID]->score += 50;
		if (redPieceSimulation.size() <= 0)
			potentialMoves[potentialMoveID]->score -= 50;

		DeleteCloneGame();
	}

	void FindBestMove(std::vector<Move*> movesToCalculate)
	{
		//random call for testing
			//int r = rand() % movesToCalculate.size();
			//MakeMove(movesToCalculate, r, m_board->blackPieces, m_board->redPieces);

		if (movesToCalculate.size() > 1)
		{
			potentialMoves = movesToCalculate;
			CloneGame(m_board->redPieces, m_board->blackPieces);

			for (int i = 0; i < potentialMoves.size(); i++)
			{
				currentPotentialMove = i;
				while (searchDepth > 0)
				{
					SimulateGame(potentialMoves);
				}
				searchDepth = SearchDepthTotal;
			}
			//find highestScore move from potential moves
			//call make move with that move
			int highestScore = 0;
			for (int i = 0; i < potentialMoves.size(); i++)
			{
				if (potentialMoves[i]->score >= highestScore)
				{
					highestScore = potentialMoves[i]->score;
					bestMove = i;
				}
			}
			MakeMove(potentialMoves, bestMove, m_board->blackPieces, m_board->redPieces, true);
		}
		else
			MakeMove(movesToCalculate, 0, m_board->blackPieces, m_board->redPieces, true);
	}

	void SimulateGame(std::vector<Move*> movesToCalculate)
	{
					//finish the  potential move
					//simulate a red move, deduct the red moves score from the potential move score
					//for each potential move, pick 4 random available moves, for ea
					//for each of these moves, add the score to the potential move score
					//
					//simulate black and red till sarchDepth reached
					//find highest score in the potential move list
					//feed the move to MakeMove fucntion
		for (int i = 0; i < movesToCalculate.size(); i++)
		{
			MakeMove(movesToCalculate, i, blackPieceSimulation, redPieceSimulation, false);
			//SimulateRedTurn(potentialMoves[currentPotentialMove], redPieceSimulation); //will modify score
			searchDepth -= 1;
			//GetValidMovesForBlack(blackPieceSimulation);

			for (int i = 0; i < playouts; i++)
			{
				int r = rand() % availableMoves.size();
				nonMandatoryMoves.push_back(availableMoves[r]);
				potentialMoves[currentPotentialMove]->score += nonMandatoryMoves.back()->score;
			}
			if (searchDepth > 0)
				SimulateGame(nonMandatoryMoves);
		}
	}

	void CloneGame(std::vector<CheckerPiece>& currentRedPieces, std::vector<CheckerPiece>& currentBlackPieces)
	{
		redPieceSimulation = currentRedPieces;
		blackPieceSimulation = currentBlackPieces;
	}

	void DeleteCloneGame()
	{
		redPieceSimulation.clear();
		blackPieceSimulation.clear();
	}

	//getavailablemovesforblack
	//for each move
	//set moves value to 0
	//PER PLAYOUT
		//clone
		//perform black
		//perform red
		//while game is not ended repeat
		//capture a piece +1
		//lose a piece -1
		//win +50
		//lose -50
	//delete clone
	//give move score

	//v>
	Piece DownRight(int column, int row, std::vector<CheckerPiece>& redPieces, std::vector<CheckerPiece>& blackPieces)
	{
		if (column + 1 > 7 || row + 1 > 7)
			return OUTOFBOARD;
		else if (TileHasPlayersPiece(column + 1, row + 1, redPieces))
			return RED;
		else if (TileHasOpponentsPiece(column + 1, row + 1, blackPieces))
			return BLACK;
		else if (!TileHasPlayersPiece(column + 1, row + 1, redPieces) && !TileHasOpponentsPiece(column + 1, row + 1, blackPieces))
			return NONE;
		else
		{
			std::cout << "DownRight Failed";
			return UNDEFINE;
		}
	}
	//v<
	Piece DownLeft(int column, int row, std::vector<CheckerPiece>& redPieces, std::vector<CheckerPiece>& blackPieces)
	{
		if (column - 1 < 0 || row + 1 > 7)
			return OUTOFBOARD;
		else if (TileHasPlayersPiece(column - 1, row + 1, redPieces))
			return RED;
		else if (TileHasOpponentsPiece(column - 1, row + 1, blackPieces))
			return BLACK;
		else if (!TileHasPlayersPiece(column - 1, row + 1, redPieces) && !TileHasOpponentsPiece(column - 1, row + 1, blackPieces))
			return NONE;
		else
		{
			std::cout << "DownLeft Failed";
			return UNDEFINE;
		}
	}
	//^>
	Piece UpRight(int column, int row, std::vector<CheckerPiece>& redPieces, std::vector<CheckerPiece>& blackPieces)
	{
		if (column + 1 > 7 || row - 1 < 0)
			return OUTOFBOARD;
		else if (TileHasPlayersPiece(column + 1, row - 1, redPieces))
			return RED;
		else if (TileHasOpponentsPiece(column + 1, row - 1, blackPieces))
			return BLACK;
		else if (!TileHasPlayersPiece(column + 1, row - 1, redPieces) && !TileHasOpponentsPiece(column + 1, row - 1, blackPieces))
			return NONE;
		else
		{
			std::cout << "UpRight Failed";
			return UNDEFINE;
		}
	}
	//^<
	Piece UpLeft(int column, int row, std::vector<CheckerPiece>& redPieces, std::vector<CheckerPiece>& blackPieces)
	{
		if (column -1 < 0 || row -1 < 0)
			return OUTOFBOARD;
		else if (TileHasPlayersPiece(column - 1, row - 1, redPieces))
			return RED;
		else if (TileHasOpponentsPiece(column - 1, row - 1, blackPieces))
			return BLACK;
		else if (!TileHasPlayersPiece(column - 1, row - 1, redPieces) && !TileHasOpponentsPiece(column - 1, row - 1, blackPieces))
			return NONE;
		else
		{
			std::cout << "UpLeft Failed";
			return UNDEFINE;
		}
	}
	bool TileHasPlayersPiece(int column, int row, std::vector<CheckerPiece>& redPieces)
	{
		// test player one
		for (int i = 0; i < redPieces.size(); i++)
		{
			if (redPieces[i].boardPosition.x == column &&
				redPieces[i].boardPosition.y == row)
				return true;
		}
		return false;
	}
	bool TileHasOpponentsPiece(int column, int row, std::vector<CheckerPiece>& blackPieces)
	{
		for (int i = 0; i < blackPieces.size(); i++)
		{
			if (blackPieces[i].boardPosition.x == column &&
				blackPieces[i].boardPosition.y == row)
				return true;
		}
		return false;
	}

	void ConsecutiveMove(int column, int row, std::vector<CheckerPiece>& redPieces, std::vector<CheckerPiece>& blackPieces)
	{
		for (int i = 0; i < blackPieces.size(); i++)
		{
			if (blackPieces[i].boardPosition == glm::vec2(column, row))
			{
				if (blackPieces[i].isKing == true)
				{
					if (DownRight(column, row, redPieces, blackPieces) == RED)
					{
						if (DownRight(column + 1, row + 1, redPieces, blackPieces) == NONE)
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
					if (DownLeft(column, row, redPieces, blackPieces) == RED)
					{
						if (DownLeft(column - 1, row + 1, redPieces, blackPieces) == NONE)
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
					if (DownRight(column, row, redPieces, blackPieces) == NONE)
					{
						Move* n = new Move();
						availableMoves.push_back(n);
						availableMoves.back()->capture = false;
						availableMoves.back()->score = 0;
						availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
						availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row + 1) * 10);
						availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row + 1));
					}
					if (DownLeft(column, row, redPieces, blackPieces) == NONE)
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
		if (UpRight(column, row, redPieces, blackPieces) == RED)
		{
			if (UpRight(column + 1, row - 1, redPieces, blackPieces) == NONE)
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
		if (UpLeft(column, row, redPieces, blackPieces) == RED)
		{
			if (UpLeft(column - 1, row - 1, redPieces, blackPieces) == NONE)
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
		if (UpRight(column, row, redPieces, blackPieces) == NONE)
		{
			Move* n = new Move();
			availableMoves.push_back(n);
			availableMoves.back()->capture = false;
			availableMoves.back()->score = 0;
			availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
			availableMoves.back()->endingPieceLocation = glm::vec3((column + 1) * 10, 0, (row - 1) * 10);
			availableMoves.back()->endingPieceBoardLocation = glm::vec2((column + 1), (row - 1));
		}
		if (UpLeft(column, row, redPieces, blackPieces) == NONE)
		{
			Move* n = new Move();
			availableMoves.push_back(n);
			availableMoves.back()->capture = false;
			availableMoves.back()->score = 0;
			availableMoves.back()->startingPieceLocation = glm::vec2(column, row);
			availableMoves.back()->endingPieceLocation = glm::vec3((column - 1) * 10, 0, (row - 1) * 10);
			availableMoves.back()->endingPieceBoardLocation = glm::vec2((column - 1), (row - 1));
		}

		for (int i = 0; i < availableMoves.size(); i++)
		{
			if (availableMoves[i]->capture == true)
				mandatoryMoves.push_back(availableMoves[i]);
		}
		if (mandatoryMoves.size() > 0)
		{
			availableMoves = mandatoryMoves;
			int r = rand() % availableMoves.size();
			MakeMove(availableMoves, r, m_board->blackPieces, m_board->redPieces, true);
		}
	}

	void DisplayDebugArray()
	{
		for (int row = 0; row < 8; row++)
		{
			for (int column = 0; column < 8; column++)
			{
				DebugArray[column][row] = 0;
				for (int r = 0; r < redPieceSimulation.size(); r++)
				{
					int x = redPieceSimulation[r].boardPosition.x;
					int y = redPieceSimulation[r].boardPosition.y;
					DebugArray[x][y] = 1;
				}
				for (int r = 0; r < blackPieceSimulation.size(); r++)
				{
					int x = blackPieceSimulation[r].boardPosition.x;
					int y = blackPieceSimulation[r].boardPosition.y;
					DebugArray[x][y] = 2;
				}
			}
		}
		for (int row = 0; row < 8; row++)
		{
			for (int column = 0; column < 8; column++)
			{
				std::cout << DebugArray[column][row] << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}

	std::vector <CaptureMove> possibleCaptures;

	std::vector<Move*> availableMoves;
	std::vector<Move*> mandatoryMoves;
	std::vector<Move*> nonMandatoryMoves;
	std::vector<Move*> potentialMoves;
	int bestMove;

	bool pieceCaptured;
	bool finalMove = false;
	bool bonusMove = false;

	std::vector<std::thread> threads;
	static std::mutex mutexLock;

private:

	CheckersBoard* m_board;

	int DebugArray[8][8];

	std::vector<CheckerPiece> redPieceSimulation;
	std::vector<CheckerPiece> blackPieceSimulation;
	BoardPiece SimulationCheckerBoard[8][8];

	int playouts;
	int playoutsTotal;
	int searchDepth;
	int SearchDepthTotal;
	int currentPotentialMove;
};