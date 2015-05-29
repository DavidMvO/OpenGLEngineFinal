#include "CheckersApplication.h"

bool CheckersApplication::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_board = new CheckersBoard();

	m_currentPlayer = PLAYER_ONE;
	m_currentOpponent = PLAYER_TWO;

	tileSelected = false;	

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
							//if you have not already selected a piece
							if (TileIsClicked(i, j) && tileSelected == false)
							{
								currentTile = &m_board->checkerBoard[i][j];
								//check clicked tile for a player piece
								if (TileHasPlayersPiece(i, j, GetCurrentGameState()))
								{
									m_board->checkerBoard[i][j].selected = true;
									m_board->checkerBoard[i][j].colour = glm::vec4(1, 1, 1, 1);
									selectedTile = &m_board->checkerBoard[i][j];
									tileSelected = true;

									//if it has a player piece, get its avaliable moves
									if (TileHasPlayersPiece(i, j, GetCurrentGameState()))
										GetAvailableMoves(i, j, GetCurrentGameState());
								}
							}
							//if you have already selected a piece
							else if (TileIsClicked(i, j) && tileSelected == true)
							{
								currentTile = &m_board->checkerBoard[i][j];
								//if clicked tile is the selected one, deselect it
								if (selectedTile == &m_board->checkerBoard[i][j])
								{
									selectedTile->selected = false;
									selectedTile->colour = glm::vec4(1, 0.25, 0.25, 1);
									currentTile = nullptr;
									for (int i = 0; i < 8; i++)
									{
										for (int j = 0; j < 8; j++)
										{
											m_board->checkerBoard[i][j].colour = m_board->checkerBoard[i][j].originalColour;
											m_board->checkerBoard[i][j].available = false;
										}
									}
									m_board->hasTilesSelected = false;
									tileSelected = false;
									m_board->availableMoves.clear();
									break;
								}
								//if clicked tile is an available move
								//find the piece on the selectedtile(white tile)
								for (int p = 0; p < m_board->redPieces.size(); p++)
								{
									if (m_board->redPieces[p].boardPosition == selectedTile->boardPosition)
									{
										//check if available move
										if (currentTile->available == true)
										{
											//move piece to currentTile(green)
											m_board->redPieces[p].boardPosition = currentTile->boardPosition;
											m_board->redPieces[p].position = currentTile->position;

											selectedTile->selected = false;
											selectedTile->colour = glm::vec4(1, 0.25, 0.25, 1);
											currentTile = nullptr;
											for (int i = 0; i < 8; i++)
											{
												for (int j = 0; j < 8; j++)
												{
													m_board->checkerBoard[i][j].colour = m_board->checkerBoard[i][j].originalColour;
													m_board->checkerBoard[i][j].available = false;
												}
											}
											m_board->hasTilesSelected = false;
											tileSelected = false;
											m_board->availableMoves.clear();
											break;
										}
									}
								}
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
					//if up and across.left is free
					if (m_board->checkerBoard[i][j].boardPosition.y == row + 1 &&
						m_board->checkerBoard[i][j].boardPosition.x == column + 1)
					{
						if (!TileHasPlayersPiece(column + 1, row + 1, currentPlayer))
						{
							m_board->availableMoves.push_back(&m_board->checkerBoard[column + 1][row + 1]);
							m_board->checkerBoard[column + 1][row + 1].available = true;
							m_board->checkerBoard[column + 1][row + 1].colour = glm::vec4(0, 1, 0, 1);
						}
					}
					//if up and across.right is free
					if (m_board->checkerBoard[i][j].boardPosition.y == row + 1 &&
						m_board->checkerBoard[i][j].boardPosition.x == column - 1)
					{
						if (!TileHasPlayersPiece(column - 1, row + 1, currentPlayer))
						{
							m_board->availableMoves.push_back(&m_board->checkerBoard[column + 1][row + 1]);
							m_board->checkerBoard[column - 1][row + 1].available = true;
							m_board->checkerBoard[column - 1][row + 1].colour = glm::vec4(0, 1, 0, 1);
						}
					}
					//check for opponent pieces for jumps
				}
			}
			m_board->hasTilesSelected = true;
	}


	//}
	// test player two
	if (currentPlayer == CheckersApplication::PLAYER_TWO)
	{

	}
}