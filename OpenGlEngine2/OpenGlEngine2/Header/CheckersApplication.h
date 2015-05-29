#ifndef CHECKERS_APPLICATION_H
#define CHECKERS_APPLICATION_H

#include "Application.h"
#include "AIBase.h"
#include "CheckersBoard.h"

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
	void GetValidActions();
	bool IsActionValid();

	void PerformAction();
	bool TileIsClicked(int row, int column);
	bool TileHasPlayersPiece(int row, int column, State currentplayer);
	bool TileHasOpponentsPiece(int row, int column, State currentplayer);
	void GetAvailableMoves(int column, int row, State currentPlayer);
	//clone current state
	//virtual CheckersApplication* Clone() const = 0;

private:

	CheckersBoard* m_board;

	State m_currentPlayer;
	State m_currentOpponent;

	glm::vec3 m_clickPosition;

	bool tileSelected;
	BoardPiece* selectedTile;
	CheckerPiece* selectedPiece;
	BoardPiece* currentTile;

};

#endif