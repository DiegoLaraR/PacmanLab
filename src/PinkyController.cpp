#include "PinkyController.h"
#include <iostream>

using namespace std;

InfoPinky* InfoPinky::info=nullptr;


PinkyController::PinkyController(std::shared_ptr<Character> character):Controller(character),root(std::make_shared<Selector>())  {

	auto filter = std::make_shared<Filter>();
	filter->addCondition(std::make_shared<PowerpillTransition>()); 	// Condition
	filter->addAction(std::make_shared<FrightenedStatePinky>()); 	
	
	auto filter2 = make_shared<Filter>();

	filter2->addCondition(make_shared<TimeOutTransition>());
	filter2->addAction(make_shared<ScatterStatePinky>());

	auto chase = make_shared<ChaseStatePinky>();

	
	// Action
	root->addChild(filter);
	root->addChild(filter2);
	root->addChild(chase);
}

PinkyController::~PinkyController() {
	// TODO Auto-generated destructor stub
}

Move PinkyController::getMove(const GameState& gs){
	InfoPinky::getInfo()->in_character=character;
	InfoPinky::getInfo()->in_gamestate=&gs;
	

	root->tick();


	return InfoPinky::getInfo()->out_move;
}

TimeOutTransition::TimeOutTransition() : Behavior() {
	lastTime = std::chrono::high_resolution_clock::now();

}

Status TimeOutTransition::update(){
	std::chrono::duration<float> timeStamp = std::chrono::high_resolution_clock::now() - lastTime;
	if( (int)timeStamp.count()%27 < 7){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}

}

Status ChaseStatePinky::update(){
	std::cerr << " Pinky Chase \n" ;
	auto gs = InfoPinky::getInfo()->in_gamestate;
	
	auto direction = gs->getPacmanDir();
	
	Move m = static_cast<Move>(direction);
	
	int node = gs->getPacmanPos();
	
	for (int i = 0; i < 4; i++) {

		int nextNode = gs->getMaze().getNeighbour(node, m);

		if (nextNode == -1) {
			break;
		}
		
		node = nextNode;
	}
	
	auto target = gs->getMaze().getNodePos(node);
	
	float min=1000000000;
	Move minMove=PASS;
	vector<Move> moves;
	auto character = InfoPinky::getInfo()->in_character;
	
	if(character->getDirection()==PASS) {
		moves=gs->getMaze().getPossibleMoves(character->getPos());
		
	} else {
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	
	for(auto move:moves) {
		if(move==PASS) {
			continue;
		}
		float dist = euclid2(target,gs->getMaze().getNodePos(gs->getMaze().getNeighbour(character->getPos(),move)));
		if(dist<min) {
			min=dist;
			minMove=move;
		}
	}
	std::cerr << "Nodo 4: " << target.first << ", " << target.second << "\n" ;

	auto pacman = gs->getMaze().getNodePos(gs->getPacmanPos());
	std::cerr << "Pacman: " << pacman.first << ", " << pacman.second << "\n" ;
	InfoPinky::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}


Status PowerpillTransition::update(){
	auto character = InfoPinky::getInfo()->in_character;
	auto ghost = dynamic_cast<Ghost*>(character.get());

	if( ghost!=nullptr && ghost->isEdible()){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}

}

FrightenedStatePinky::FrightenedStatePinky() : Behavior(), e(rand()), uniform_dist(0,3){

}

Status FrightenedStatePinky::update(){
	std::cerr << "Pinky Frightened \n" ;
	auto character = InfoPinky::getInfo()->in_character;
	auto gs = InfoPinky::getInfo()->in_gamestate;
	std::vector<Move> moves;
	if(character->getDirection()==PASS) {
		moves=gs->getMaze().getPossibleMoves(character->getPos());
	} else {
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
	}
	Move m = moves[rand()%moves.size()];
	InfoPinky::getInfo()->out_move = m;
	return BH_SUCCESS; //NO es as� pero por ahora
}

ScatterStatePinky :: ScatterStatePinky() : Behavior(){
	target = std::make_pair(-1,-1);

}

Status ScatterStatePinky::update(){
	std::cerr << "Pinky Scatter \n" ;
	
	auto character = InfoPinky::getInfo()->in_character;
	auto gs = InfoPinky::getInfo()->in_gamestate;

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
	InfoPinky::getInfo()->out_move = minMove;
	return BH_SUCCESS;

}

