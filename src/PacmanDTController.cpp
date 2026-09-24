#include "PacmanDTController.h" 
#include <iostream>
#include <SDL2/SDL.h>
//#include "GameState.h"

using namespace std;

InfoPacman* InfoPacman::info = nullptr;

PacmanDTController::PacmanDTController(std::shared_ptr<Character> character): Controller(character), root(make_shared<Selector>())
{
	auto frigtned = make_shared<Filter>();
	
	frigtned->addCondition(make_shared<FrigtnedTransitionPacman>());
	frigtned->addAction(make_shared<FrigtnedPacman>());
	
	auto eatGhostPacman = make_shared<Filter>();
	
	eatGhostPacman->addCondition(make_shared<EatGhostTransition>());
	eatGhostPacman->addAction(make_shared<EatGhostPacman>());
	
	auto eatPowerPills = make_shared<Filter>();
	
	eatPowerPills->addCondition(make_shared<PowerPillTransition>());
	eatPowerPills->addAction(make_shared<EatPowerPillPacman>());
	
	auto eatPills = make_shared<Filter>();
	
	eatPills->addAction(make_shared<EatPillPacman>());

	root->addChild(frigtned);
	root->addChild(eatGhostPacman);
	root->addChild(eatPowerPills);
	root->addChild(eatPills);

} 
PacmanDTController::~PacmanDTController() { } 

Move PacmanDTController::getMove(const GameState& gs){

	SDL_Event e;
    // Manejar todos los eventos en la cola por cada frame
    while(SDL_PollEvent(&e) != 0)
    {
        if(e.type == SDL_QUIT)
        {
            SDL_Quit();
            exit(0);
        }
        
        if(e.type == SDL_KEYDOWN)
        {
            if(e.key.keysym.sym == SDLK_ESCAPE)
            {
                SDL_Quit();
                exit(0);
            }
        }
    }

	InfoPacman::getInfo()->in_character=character;
	InfoPacman::getInfo()->in_gamestate=&gs;
	
	root->tick();

	return InfoPacman::getInfo()->out_move;
}



FrigtnedTransitionPacman::FrigtnedTransitionPacman() : Behavior(){}

Status FrigtnedTransitionPacman::update()
{
	auto gs = InfoPacman::getInfo()->in_gamestate;
	auto pacmanCoords = gs->getMaze().getNodePos(gs->getPacmanPos());
	
	float limit = isFleeing ? 100.0f : 49.0f;

	isFleeing = false;

	for(int i = 0; i < 4; i++)
	{
		if(gs->isGhostEdible(i))
			continue;

		auto ghost = gs->getMaze().getNodePos(gs->getGhostsPos(i));

		float dist = euclid2(ghost, pacmanCoords);

		if(dist <= limit)
		{
			isFleeing = true;
			return BH_SUCCESS;
		}
	}
	return BH_FAILURE;
}


FrigtnedPacman::FrigtnedPacman() : Behavior()
{}

Status FrigtnedPacman::update()
{
	vector<Move> moves;
	auto gs = InfoPacman::getInfo()->in_gamestate;
	
	auto pacmanPos = gs->getPacmanPos();
	int maxDist=-1;
	Move maxMove = PASS; 
	moves = gs->getMaze().getPossibleMoves(pacmanPos);
	
	for(Move m:moves){
		
		int vecino = gs->getMaze().getNeighbour(pacmanPos, m);
		if(vecino<0) continue;
	
		auto vecinoCoords = gs->getMaze().getNodePos(vecino);		
		int currentMoveMinDist = 999999;

		for( int i = 0; i < 4; i++ )
		{
			if(gs->isGhostEdible(i)) continue;

			auto ghostCoords = gs->getMaze().getNodePos(gs->getGhostsPos(i));
			auto dist = euclid2(ghostCoords, vecinoCoords);

			if(dist < currentMoveMinDist)
			{
				currentMoveMinDist = dist;
			}
		}

		Move currentDir = InfoPacman::getInfo()->in_character->getDirection();
		Move opossite = PASS;

		switch(currentDir)
		{
		case UP:
			opossite = DOWN;
			break;
		case DOWN:
			opossite = UP;
			break;
		case LEFT:
			opossite = RIGHT;
			break;
		case RIGHT:
			opossite = LEFT;
			break;
		default:
			opossite = PASS;
			break;
		}
		if(m == opossite){currentMoveMinDist -= 1000;}


		if(currentMoveMinDist > maxDist)
		{
			maxDist = currentMoveMinDist;
			maxMove = m;
		}
	}

	InfoPacman::getInfo()->out_move = maxMove;

	return BH_SUCCESS;
}





EatPillPacman::EatPillPacman() : Behavior()
{}

Status EatPillPacman::update()
{
	auto gs = InfoPacman::getInfo()->in_gamestate;
	
	auto pacmanPos = gs->getPacmanPos();
	auto pacmanCoords = gs->getMaze().getNodePos(pacmanPos);
	
	
	auto pills = gs->getMaze().getPillPositions();
	
	if(pills.empty())
	{
		InfoPacman::getInfo()->out_move = PASS;
		return BH_FAILURE;
	}
	
	float minDist = 999999;
	pair<int, int> nearestPill;

	for (auto pill : pills)
	{
		int dist = euclid2(pill, pacmanCoords);

		if(dist < minDist)
		{
			minDist = dist;
			nearestPill = pill;
		}	
	}

	int minMoveDist = 999999;
	Move minMove = PASS;

	auto moves = gs->getMaze().getPossibleMoves(pacmanPos);

	Move currentDir = InfoPacman::getInfo()->in_character->getDirection();
	Move opossite = PASS;

	switch(currentDir)
	{
		case UP:
			opossite = DOWN;
			break;
		case DOWN:
			opossite = UP;
			break;
		case LEFT:
			opossite = RIGHT;
			break;
		case RIGHT:
			opossite = LEFT;
			break;
		default:
			opossite = PASS;
			break;
	}

	

	for(Move m : moves)
	{
		int vecino = gs->getMaze().getNeighbour(pacmanPos, m);
		
		if(vecino < 0)
			continue;

		auto vecinoCoords = gs->getMaze().getNodePos(vecino);
		int dist = euclid2(nearestPill, vecinoCoords);

		if(m == opossite){dist += 1000;}

		if(dist < minMoveDist)
		{
			minMoveDist = dist;
			minMove = m;
		}
	}
	
	InfoPacman::getInfo()->out_move = minMove;

	return BH_SUCCESS;
}


PowerPillTransition::PowerPillTransition() : Behavior()
{}

Status PowerPillTransition::update()
{
	auto gs = InfoPacman::getInfo()->in_gamestate;
	auto pacmanCoords = gs->getMaze().getNodePos(gs->getPacmanPos());
	
	auto powerPills = gs->getMaze().getPowerPillPositions();

	for(auto pill : powerPills)
	{
		int dist = euclid2(pill, pacmanCoords);

		if(dist <= 25)
		{
			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}

EatPowerPillPacman::EatPowerPillPacman() : Behavior(){}

Status EatPowerPillPacman::update()
{
	auto gs = InfoPacman::getInfo()->in_gamestate;
	
	auto pacmanPos = gs->getPacmanPos();
	auto pacmanCoords = gs->getMaze().getNodePos(pacmanPos);
	
	auto powerPills = gs->getMaze().getPowerPillPositions();
	
	if(powerPills.empty())
	{
		InfoPacman::getInfo()->out_move = PASS;
		return BH_FAILURE;
	}
	
	float minDist = 999999;
	pair<int, int> nearestPill;

	for (auto pill : powerPills)
	{
		int dist = euclid2(pill, pacmanCoords);

		if(dist < minDist)
		{
			minDist = dist;
			nearestPill = pill;
		}	
	}

	int minMoveDist = 999999;
	Move minMove = PASS;

	auto moves = gs->getMaze().getPossibleMoves(pacmanPos);

	for(Move m : moves)
	{
		int vecino = gs->getMaze().getNeighbour(pacmanPos, m);
		
		if(vecino < 0)
			continue;

		auto vecinoCoords = gs->getMaze().getNodePos(vecino);

		int dist = euclid2(nearestPill, vecinoCoords);

		if(dist < minMoveDist)
		{
			minMoveDist = dist;
			minMove = m;
		}
	}
	
	InfoPacman::getInfo()->out_move = minMove;

	return BH_SUCCESS;
}

EatGhostTransition::EatGhostTransition() : Behavior()
{}

Status EatGhostTransition::update()
{
	auto gs = InfoPacman::getInfo()->in_gamestate;
	auto pacmanCoords = gs->getMaze().getNodePos(gs->getPacmanPos());

	for(int i = 0; i < 4; i++)
	{
		if(!gs->isGhostEdible(i))
			continue;
		
		auto ghostCoords = gs->getMaze().getNodePos(gs->getGhostsPos(i));
		float dist = euclid2(ghostCoords, pacmanCoords);

		if(dist <= 49)
		{
			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}

EatGhostPacman::EatGhostPacman() : Behavior()
{}

Status EatGhostPacman::update()
{
	auto gs = InfoPacman::getInfo()->in_gamestate;
	auto pacmanPos = gs->getPacmanPos();
	auto pacmanCoords = gs->getMaze().getNodePos(pacmanPos);

	int nearestGhost = -1;
	float minDist = 999999;

	for(int i = 0; i < 4; i++)
	{
		if(gs->isGhostEdible(i))
		{
			auto ghostCoords = gs->getMaze().getNodePos(gs->getGhostsPos(i));
			float dist = euclid2(ghostCoords, pacmanCoords);

			if(dist < minDist)
			{
				minDist = dist;
				nearestGhost = i;
			}
		}
	}

	if(nearestGhost == -1) return BH_FAILURE;

	auto targetGhostCoords = gs->getMaze().getNodePos(gs->getGhostsPos(nearestGhost));

	int minMoveDist = 999999;
	Move bestMove = PASS;
	auto moves = gs->getMaze().getPossibleMoves(pacmanPos);

	for(Move m : moves)
	{
		int vecino = gs->getMaze().getNeighbour(pacmanPos, m);
		if(vecino < 0) continue;

		auto vecinoCoords = gs->getMaze().getNodePos(vecino);
		int dist = euclid2(targetGhostCoords, vecinoCoords);

		if(dist < minMoveDist)
		{
			minMoveDist = dist;
			bestMove = m;
		}
	}

	InfoPacman::getInfo()->out_move = bestMove;
	return BH_SUCCESS;
}