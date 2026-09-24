#include "InkyController.h"

#include <iostream>

using namespace std;

InfoInky* InfoInky::info=nullptr;


InkyController::InkyController(std::shared_ptr<Character> character):Controller(character),root(std::make_shared<Selector>())  {

	auto filter = std::make_shared<Filter>();
	filter->addCondition(std::make_shared<PowerpillInky>()); 	// Condition
	filter->addAction(std::make_shared<FrightenedInky>()); 	
	
	auto filter2 = make_shared<Filter>();

	filter2->addCondition(make_shared<TimeOutInky>());
	filter2->addAction(make_shared<ScatterInky>());

	auto chase = make_shared<ChaseInky>();

	
	// Action
	// root->addChild(newBehavior);
	root->addChild(filter);
	root->addChild(filter2);
	root->addChild(chase);
}

InkyController::~InkyController() {
	// TODO Auto-generated destructor stub
}

Move InkyController::getMove(const GameState& gs){
	InfoInky::getInfo()->in_character=character;
	InfoInky::getInfo()->in_gamestate=&gs;
	

	root->tick();


	return InfoInky::getInfo()->out_move;
}

TimeOutInky::TimeOutInky() : Behavior() {
	lastTime = std::chrono::high_resolution_clock::now();

}

Status TimeOutInky::update(){
	std::chrono::duration<float> timeStamp = std::chrono::high_resolution_clock::now() - lastTime;
	if( (int)timeStamp.count()%27 < 7){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}
}

Status ChaseInky::update(){
	std::cerr << "Inky Chase \n" ;
	auto gs = InfoInky::getInfo()->in_gamestate;
	
	// auto pacmanPos = gs->getMaze().getNodePos(gs->getPacmanPos());

	//direccion de pacman
	auto direction = gs->getPacmanDir();
	Move m = static_cast<Move>(direction);
	
	//nodo de pacman
	int pacmanPos = gs->getPacmanPos();
	
	
	// Se obtienen las dos casillas adelante de pacman, segun la direccion que esta mirando
	if(m != PASS){
		for (int i = 0; i < 2; i++) {

			int nextNode = gs->getMaze().getNeighbour(pacmanPos, m);

			if (nextNode == -1) {
				break;
			}
			
			pacmanPos = nextNode;
		}
	}
	if(pacmanPos == -1)
	{
		pacmanPos = gs->getPacmanPos();
	}
	 
	auto targetTemp = gs->getMaze().getNodePos(pacmanPos);

	auto blinkyPos = gs->getMaze().getNodePos(gs->getGhostsPos(0));

	int dx = targetTemp.first - blinkyPos.first;
	int dy = targetTemp.second - blinkyPos.second;

	auto target = make_pair
	(
		blinkyPos.first + dx * 2,
		blinkyPos.second + dy * 2
	);
	
	float min=1000000000;
	Move minMove=PASS;
	vector<Move> moves;
	auto character = InfoInky::getInfo()->in_character;
	
	if(character->getDirection()==PASS) {
		moves=gs->getMaze().getPossibleMoves(character->getPos());
		
	} else {
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	
	for(auto move:moves) {
		if(move==PASS) {
			continue;
		}

		int vecino = gs->getMaze().getNeighbour(character->getPos(),move);
		
		if(vecino < 0)
		{
			continue;
		}

		float dist = euclid2(target, gs->getMaze().getNodePos(vecino));
		if(dist<min) {
			min=dist;
			minMove=move;
		}
	}
	std::cerr << "Nodo 4: " << target.first << ", " << target.second << "\n" ;

	auto pacman = gs->getMaze().getNodePos(gs->getPacmanPos());
	std::cerr << "Pacman: " << pacman.first << ", " << pacman.second << "\n" ;
	InfoInky::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}


Status PowerpillInky::update(){
	auto character = InfoInky::getInfo()->in_character;
	auto ghost = dynamic_cast<Ghost*>(character.get());

	if( ghost!=nullptr && ghost->isEdible()){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}

}

FrightenedInky::FrightenedInky() : Behavior(), e(rand()), uniform_dist(0,3){

}

Status FrightenedInky::update(){
	std::cerr << "Inky Frightened \n" ;
	auto character = InfoInky::getInfo()->in_character;
	auto gs = InfoInky::getInfo()->in_gamestate;
	std::vector<Move> moves;
	if(character->getDirection()==PASS) {
		moves=gs->getMaze().getPossibleMoves(character->getPos());
	} else {
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	Move m = moves[rand()%moves.size()];
	InfoInky::getInfo()->out_move = m;
	return BH_SUCCESS;
}

ScatterInky :: ScatterInky() : Behavior(){
	target = std::make_pair(104,116);

}

Status ScatterInky::update(){
	std::cerr << "Inky Scatter \n" ;

	auto character = InfoInky::getInfo()->in_character;
	auto gs = InfoInky::getInfo()->in_gamestate;

	Move minMove=PASS;
	std::vector<Move> moves;
	if(character->getDirection()==PASS) {
		moves=gs->getMaze().getPossibleMoves(character->getPos());
	} else {
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	
	float min=100000000;
	for(auto move:moves) {
		if(move==PASS) {
			continue;;
		}
		float dist = euclid2(target,gs->getMaze().getNodePos(gs->getMaze().getNeighbour(character->getPos(),move)));
		if(dist<min) {
			min=dist;
			minMove=move;
		}
	}
	InfoInky::getInfo()->out_move = minMove;
	return BH_SUCCESS;

}

