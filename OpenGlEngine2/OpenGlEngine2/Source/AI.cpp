//#include "CheckersApplication.h"
//
//void RandomAI::MakeDecision(CheckersApplication& game)
//{
//	//get all availalbe moves for black pieces
//	for (int j = 0; j < 8; j++)
//	{
//		for (int i = 0; i < 8; i++)
//		{
//			for (int p = 0; p < game.GetBoard()->blackPieces.size(); p++)
//			{
//				if (game.GetBoard()->blackPieces[p].boardPosition == glm::vec2(i, j))
//				{
//					game.GetAvailableMoves(i, j, game.GetCurrentGameState());
//				}
//			}
//		}
//	}
//}