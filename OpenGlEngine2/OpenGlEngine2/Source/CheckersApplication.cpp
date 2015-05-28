#include "CheckersApplication.h"

bool CheckersApplication::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_board = new CheckersBoard();

	m_currentPlayer = PLAYER_ONE;
	m_currentOpponent = PLAYER_TWO;
	

	return true;

}

void CheckersApplication::ShutDown()
{
	delete m_board;
}

bool CheckersApplication::Update(double dt)
{
	// kepp track of if the mouse has been clicked
	static bool moveMade = false;

		// if it is the user's turn (player 1)
		if (GetCurrentGameState() == CheckersApplication::PLAYER_ONE) 
		{
			// if the mouse button is down...
			if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS) 
			{
				// if the mouse JUST went down then make a choice
				if (moveMade == false) 
				{
					moveMade = true;
					// get the mouse position within the game grid
					double x = 0, y = 0;
					glfwGetCursorPos(m_window, &x, &y);
					m_clickPosition = m_camera->PickAgainstPlane((float)x, (float)y, glm::vec4(0, 1, 0, 0));
					Gizmos::addDisk(m_clickPosition, 4, 20, glm::vec4(0, 1, 0, 1));
					//get selected board tile
					for (int j = 0; j < 8; j++)
					{
						for (int i = 0; i < 8; i++)
						{
							if (TileIsClicked(i, j))
							{
									m_board->checkerBoard[i][j].selected = TileHasPlayersPiece(i, j, GetCurrentGameState());
									BoardPiece SelectedTile = m_board->checkerBoard[i][j];

									if (TileHasPlayersPiece(i, j, GetCurrentGameState()))
										GetAvailableMoves(i, j, GetCurrentGameState());
							}
						}
					}

					//check available moves for selected piece

					//if available move selected, 
					//PerformAction();
					 
					//if piece selected again, deselect
				}
			}
			else
				moveMade = false;
		}
		else 
		{
			// it is the opponent's turn (player 2)
			// use the A.I. to make a decision
		}

	m_board->UpdateBoard();

	m_camera->Update(dt);
	return true;
}

void CheckersApplication::Render()
{

}

void CheckersApplication::GetValidActions()
{

}

bool CheckersApplication::IsActionValid()
{
	return true;
}

void CheckersApplication::PerformAction()
{
	// switch players around
	m_currentPlayer = (m_currentPlayer == CheckersApplication::PLAYER_TWO) ? CheckersApplication::PLAYER_ONE : CheckersApplication::PLAYER_TWO;
	m_currentOpponent = (m_currentOpponent == CheckersApplication::PLAYER_TWO) ? CheckersApplication::PLAYER_ONE : CheckersApplication::PLAYER_TWO;
}

CheckersApplication::State CheckersApplication::GetCurrentGameState()
{
		// test player two
		if (m_currentPlayer == CheckersApplication::PLAYER_TWO)
		{
			return CheckersApplication::PLAYER_TWO;
		}
		// test player one
		if (m_currentPlayer == CheckersApplication::PLAYER_ONE)
		{
			return CheckersApplication::PLAYER_ONE;
		}

		// no winner
		return CheckersApplication::UNKNOWN;

}

bool CheckersApplication::TileIsClicked(int column, int row)
{
	if (m_clickPosition.x < m_board->checkerBoard[column][row].position.x + 5 &&
		m_clickPosition.x > m_board->checkerBoard[column][row].position.x - 5 &&
		m_clickPosition.z < m_board->checkerBoard[column][row].position.z + 5 &&
		m_clickPosition.z > m_board->checkerBoard[column][row].position.z - 5)
		return true;
	else
		return false;
}

bool CheckersApplication::TileHasPlayersPiece(int column, int row, State currentPlayer)
{
	// test player one
	if (currentPlayer == CheckersApplication::PLAYER_ONE)
	{
		for (int i = 0; i < m_board->redPieces.size(); i++)
		{
			if (m_board->redPieces[i].boardPosition.x == column &&
				m_board->redPieces[i].boardPosition.y == row)
				return true;
		}
		return false;
	}
}

bool CheckersApplication::TileHasOpponentsPiece(int column, int row, State currentPlayer)
{
	if (currentPlayer == CheckersApplication::PLAYER_ONE)
	{
		for (int i = 0; i < m_board->blackPieces.size(); i++)
		{
			if (m_board->blackPieces[i].boardPosition.x == column &&
				m_board->blackPieces[i].boardPosition.y == row)
				return true;
		}
		return false;
	}
}

void CheckersApplication::GetAvailableMoves(int column, int row, State currentPlayer)
{
	// test player one
	if (currentPlayer == CheckersApplication::PLAYER_ONE)
	{
			for (int j = 0; j < 8; j++)
			{
				for (int i = 0; i < 8; i++)
				{
					if (m_board->checkerBoard[i][j].boardPosition.y == row + 1 &&
						m_board->checkerBoard[i][j].boardPosition.x == column + 1)
					{
						bool test = TileHasPlayersPiece(column + 1, row + 1, currentPlayer);
						if (!test)
						{
							m_board->availableMoves.push_back(m_board->checkerBoard[column + 1][row + 1]);
							m_board->checkerBoard[column + 1][row + 1].available = true;
						}
					}
					if (m_board->checkerBoard[i][j].boardPosition.y == row + 1 &&
						m_board->checkerBoard[i][j].boardPosition.x == column - 1)
					{
						if (!TileHasPlayersPiece(column + 1, row + 1, currentPlayer))
						{
							m_board->availableMoves.push_back(m_board->checkerBoard[column + 1][row + 1]);
							m_board->checkerBoard[column - 1][row + 1].available = true;
						}
					}
				}
			}
	}


	//}
	// test player two
	if (currentPlayer == CheckersApplication::PLAYER_TWO)
	{

	}
}