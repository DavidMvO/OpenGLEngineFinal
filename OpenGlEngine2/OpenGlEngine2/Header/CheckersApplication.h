#ifndef CHECKERS_APPLICATION_H
#define CHECKERS_APPLICATION_H

#include "Application.h"
#include "CheckersBoard.h"
#include "AIBase.h"

class CheckersApplication : public Application
{
public:

	enum State 
	{
		UNKNOWN,
		PLAYER_ONE,
		PLAYER_TWO,
		DRAW,
	};

	CheckersApplication() : Application("Checkers", 1720, 1280) {}
	~CheckersApplication() {}

	virtual bool StartUp();
	virtual void ShutDown();

	//----generalfunctions------
	virtual bool Update(double dt);
	virtual void Render();

	//AI AND STATE CHANGES
	State GetCurrentGameState();
	State GetCurrentPlayer() const { return m_currentPlayer; }
	State GetCurrentOpponent() const { return m_currentOpponent; }

	//get list of valid actions for current player
	void GetValidActions(std::vector<BoardPiece*> available);
	bool IsActionValid();
	CheckersBoard* GetBoard();

	void PerformAction();
	bool TileIsClicked(int row, int column);
	void GetAvailableMoves(int column, int row, State currentPlayer);

private:

	CheckersBoard* m_board;
	AI* m_AI;

	State m_currentPlayer;
	State m_currentOpponent;

	glm::vec3 m_clickPosition;

	bool tileSelected;
	bool captureMove;
	bool mandatoryMove;
	BoardPiece* selectedTile;
	CheckerPiece* selectedPiece;
	BoardPiece* currentTile;

	bool GameOver = false;
};

#endif