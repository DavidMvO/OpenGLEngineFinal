#include "CheckersApplication.h"

bool CheckersApplication::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_board = new CheckersBoard();
	m_AI = new AI(m_board, 5, 25);

	m_currentPlayer = PLAYER_ONE;
	m_currentOpponent = PLAYER_TWO;

	//m_game = new Game();

	tileSelected = false;	
	mandatoryMove = false;

	return true;

}

void CheckersApplication::ShutDown()
{
	delete m_AI;
	delete m_board;
}

bool CheckersApplication::Update(double dt)
{
	if (m_board->blackPieces.size() == 0 || m_board->redPieces.size() == 0)
		GameOver = true;

	// kepp track of if the mouse has been clicked
	static bool moveMade = false;

		// if it is the user's turn (player 1)
		if (GetCurrentGameState() == CheckersApplication::PLAYER_ONE) 
		{
			// -----if the mouse button is down...----------------------------------------------
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
								if (m_AI->TileHasPlayersPiece(i, j, m_board->redPieces))
								{
									m_board->checkerBoard[i][j].selected = true;
									m_board->checkerBoard[i][j].colour = glm::vec4(1, 1, 1, 1);
									selectedTile = &m_board->checkerBoard[i][j];
									tileSelected = true;

									if (m_AI->TileHasPlayersPiece(i, j, m_board->redPieces))
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
									break;
								}
								//if clicked tile is an available move
								//find the piece on the selectedtile(white tile)
								for (int p = 0; p < m_board->redPieces.size(); p++)
								{
									GetAvailableMoves(m_board->redPieces[p].boardPosition.x, m_board->redPieces[p].boardPosition.y, GetCurrentGameState());
									if (m_board->redPieces[p].boardPosition == selectedTile->boardPosition)
									{
										//check for mandatoryMove
										//----
										if (m_AI->possibleCaptures.size() > 0)
										{
											for (int i = 0; i < m_AI->possibleCaptures.size(); i++)
											{
												if (currentTile->position == m_AI->possibleCaptures[i].CaptureMoveLocation)
												{
													for (int j = 0; j < m_board->blackPieces.size(); j++)
													{
														if (m_board->blackPieces[j].boardPosition == m_AI->possibleCaptures[i].CapturedPieceLocation)
														{
															//move piece to currentTile(green)
															m_board->redPieces[p].boardPosition = currentTile->boardPosition;
															m_board->redPieces[p].position = currentTile->position;
															m_board->blackPieces.erase(m_board->blackPieces.begin() + j);

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
															m_AI->possibleCaptures.clear();
															m_AI->availableMoves.clear();
															PerformAction();
															break;
														}
													}
												}
											}
										}
										//check if available move
										else if (currentTile->available == true && m_AI->possibleCaptures.size() == 0)
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
											m_AI->possibleCaptures.clear();
											m_AI->availableMoves.clear();
											PerformAction();
											break;
										}
									}
								}
							}
						}
					} 
				}
			}
			else
				moveMade = false;
		}
		else
		{
			if (GameOver != true)
			{
				//m_AI->GetValidMovesForBlack(m_board->blackPieces);
				m_AI->CalculatePotentialMoves();
				PerformAction();
			}
		}

	m_board->UpdateBoard();

	m_camera->Update(dt);
	return true;
}

void CheckersApplication::Render()
{

}

CheckersBoard* CheckersApplication::GetBoard()
{
	return m_board;
}

void CheckersApplication::GetValidActions(std::vector<BoardPiece*> available)
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


void CheckersApplication::GetAvailableMoves(int column, int row, State currentPlayer)
{
	// test player one
	if (currentPlayer == CheckersApplication::PLAYER_ONE)
	{
		//test for king
		for (int i = 0; i < m_board->redPieces.size(); i++)
		{
			if (m_board->redPieces[i].boardPosition == glm::vec2(column, row))
			{
				if (m_board->redPieces[i].isKing == true)
				{
					if (m_AI->UpRight(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->BLACK)
					{
						if (m_AI->UpRight(column + 1, row - 1, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
						{
							m_board->checkerBoard[column + 2][row - 2].available = true;
							//m_board->checkerBoard[column + 2][row - 2].colour = glm::vec4(0, 1, 0, 1);
							CaptureMove n;
							m_AI->possibleCaptures.push_back(n);
							m_AI->possibleCaptures.back().CapturedPieceLocation = glm::vec2(column + 1, row - 1);
							m_AI->possibleCaptures.back().CaptureMoveLocation = glm::vec3((column + 2) * 10, 0, (row - 2) * 10);
						}
					}
					if (m_AI->UpLeft(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->BLACK)
					{
						if (m_AI->UpLeft(column - 1, row - 1, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
						{
							m_board->checkerBoard[column - 2][row - 2].available = true;
							//m_board->checkerBoard[column - 2][row - 2].colour = glm::vec4(0, 1, 0, 1);
							CaptureMove n;
							m_AI->possibleCaptures.push_back(n);
							m_AI->possibleCaptures.back().CapturedPieceLocation = glm::vec2(column - 1, row - 1);
							m_AI->possibleCaptures.back().CaptureMoveLocation = glm::vec3((column - 2) * 10, 0, (row - 2) * 10);
						}
					}
					if (m_AI->UpRight(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
					{
						m_board->checkerBoard[column + 1][row - 1].available = true;
						//m_board->checkerBoard[column + 1][row - 1].colour = glm::vec4(0, 1, 0, 1);
					}
					if (m_AI->UpLeft(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
					{
						m_board->checkerBoard[column - 1][row - 1].available = true;
						//m_board->checkerBoard[column - 1][row - 1].colour = glm::vec4(0, 1, 0, 1);
					}
				}
			}
		}
		//red player moves, non kings
		if (m_AI->DownRight(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->BLACK)
		{
			if (m_AI->DownRight(column + 1, row + 1, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
			{
				m_board->checkerBoard[column + 2][row + 2].available = true;
				//m_board->checkerBoard[column + 2][row + 2].colour = glm::vec4(0, 1, 0, 1);
				CaptureMove n;
				m_AI->possibleCaptures.push_back(n);
				m_AI->possibleCaptures.back().CapturedPieceLocation = glm::vec2(column + 1, row + 1);
				m_AI->possibleCaptures.back().CaptureMoveLocation = glm::vec3((column + 2) * 10, 0, (row + 2) * 10);
			}
		}
		if (m_AI->DownLeft(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->BLACK)
		{
			if (m_AI->DownLeft(column - 1, row + 1, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
			{
				m_board->checkerBoard[column - 2][row + 2].available = true;
				//m_board->checkerBoard[column - 2][row + 2].colour = glm::vec4(0, 1, 0, 1);
				CaptureMove n;
				m_AI->possibleCaptures.push_back(n);
				m_AI->possibleCaptures.back().CapturedPieceLocation = glm::vec2(column - 1, row + 1);
				m_AI->possibleCaptures.back().CaptureMoveLocation = glm::vec3((column - 2) * 10, 0, (row + 2) * 10);
			}
		}
		if (m_AI->DownRight(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
		{
			m_board->checkerBoard[column + 1][row + 1].available = true;
			//m_board->checkerBoard[column + 1][row + 1].colour = glm::vec4(0, 1, 0, 1);
		}
		if (m_AI->DownLeft(column, row, m_board->redPieces, m_board->blackPieces) == m_AI->NONE)
		{
			m_board->checkerBoard[column - 1][row + 1].available = true;
			//m_board->checkerBoard[column - 1][row + 1].colour = glm::vec4(0, 1, 0, 1);
		}
		//check for opponent pieces for jumps
		m_board->hasTilesSelected = true;
	}
}

//Game* CheckersApplication::Clone()
//{
//	Game* game = new Game();
//	game->m_currentPlayer = m_game->m_currentPlayer;
//	game->m_currentOpponent = m_game->m_currentOpponent;
//	game->m_board->blackPieces = this->m_board->blackPieces;
//	game->m_board->redPieces = this->m_board->redPieces;
//	game->m_board = this->m_board;
//	return game;
//}