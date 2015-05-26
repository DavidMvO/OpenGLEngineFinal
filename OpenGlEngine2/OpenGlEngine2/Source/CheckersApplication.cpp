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
					Gizmos::addDisk(m_clickPosition, 5, 40, glm::vec4(0, 1, 0, 1));
					//get selected piece
					//check available moves for selected piece

					//if available move selected, 
					PerformAction();

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