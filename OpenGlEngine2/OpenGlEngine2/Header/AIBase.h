//
//#include <vector>
//#include <stdlib.h>
//
//// base class for an A.I. opponent
//class AI {
//public:
//	AI() {}
//	virtual ~AI() {}
//	// makes a decision for the current game
//	virtual void MakeDecision(const CheckersApplication& game) = 0;
//};
//
//// an A.I. opponent that chooses purely random actions to perform
//class RandomAI : public AI {
//public:
//	RandomAI() {}
//	virtual ~RandomAI() {}
//	// randomly choose an action to perform
//	virtual void MakeDecision(const CheckersApplication& game) 
//	{
//		std::vector<int> actions;
//		game.GetValidActions(actions);
//		if (actions.size() > 0)
//			return actions[rand() % actions.size()];
//		else
//			return -1;
//	}
//};