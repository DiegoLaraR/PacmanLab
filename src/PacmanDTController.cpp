#include "PacmanDTController.h" 
#include <iostream> 
#include <SDL2/SDL.h> 

PacmanDTController::PacmanDTController(std::shared_ptr<Character> character): Controller(character){ } 
PacmanDTController::~PacmanDTController() { } 

Move PacmanDTController::getMove(const GameState& game)
{ 
	SDL_Event e;
	if( SDL_PollEvent( &e ) != 0 ) 
	{ 
		if( e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && (e.key.keysym.sym==SDLK_ESCAPE || e.key.keysym.sym==SDLK_q) )) 
		{ 
			SDL_Quit(); 
			exit(0); 
		} 
	} 
	
	int pacmanNode = character->getPos(); 
	auto pacmanCoords = game.getMaze().getNodePos(pacmanNode); 
	int nearestGhost = 0; 
	int minDist = 10000000; 
	
	for (int i = 0; i < 4; i++) 
	{ 
		auto gCoords = game.getMaze().getNodePos(game.getGhostsPos(i)); 
		int distX = gCoords.first - pacmanCoords.first; 
		int distY = gCoords.second - pacmanCoords.second; 
		int sqDist = distX * distX + distY * distY; 
		if (sqDist < minDist) 
		{ 
			minDist = sqDist; 
			nearestGhost = i; 
		} 
	} 
	
	auto target = game.getMaze().getNodePos(game.getGhostsPos(nearestGhost)); 
	if(game.isGhostEdible(nearestGhost))
	{ 
		int minDist = 10000000; 
		Move minMove = PASS; 
		std::vector<Move> moves = game.getMaze().getPossibleMoves(pacmanNode); 
		for(Move m : moves)
		{ 
			int vecino = game.getMaze().getNeighbour(pacmanNode, m); 
			if(vecino < 0) 
				continue; 
			
			auto vecinoCoords = game.getMaze().getNodePos(vecino); 
			vecinoCoords.first -= target.first; 
			vecinoCoords.second -= target.second; 
			int sqDist = vecinoCoords.first * vecinoCoords.first + vecinoCoords.second * vecinoCoords.second; 
			if(sqDist < minDist) 
			{ 
				minDist = sqDist; 
				minMove = m; 
			} 
		} 
		
		return minMove; 
	} 
	else 
	{ 
		int maxDist =- 1; 
		Move maxMove = PASS; 
		std::vector<Move> moves = game.getMaze().getPossibleMoves(pacmanNode); 
		for(Move m : moves) 
		{ 
			int vecino = game.getMaze().getNeighbour(pacmanNode, m); 
			if(vecino < 0) 
				continue; 
			
			auto vecinoCoords = game.getMaze().getNodePos(vecino); 
			vecinoCoords.first -= target.first; 
			vecinoCoords.second -= target.second; 
			int sqDist = vecinoCoords.first * vecinoCoords.first + vecinoCoords.second * vecinoCoords.second; 
			if(sqDist > maxDist)
			{ 
				maxDist = sqDist; 
				maxMove = m; 
			} 
		} 
		return maxMove; 
	} 
}
