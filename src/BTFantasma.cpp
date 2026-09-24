#include "BTFantasma.h"

#include <iostream>

using namespace std;

InfoBTFantasma* InfoBTFantasma::info=nullptr;

// Reemplaza a Inky 

BTFantasma::BTFantasma(std::shared_ptr<Character> character):Controller(character),root(std::make_shared<Selector>())  {


	auto newBehavior = std::make_shared<Filter>();

	newBehavior->addCondition(make_shared<GuardPowerPillCondition>());
	newBehavior->addAction(make_shared<GuardPowerPill>());
	
	auto filter = std::make_shared<Filter>();
	filter->addCondition(std::make_shared<PowerpillBTFantasma>()); 	// Condition
	filter->addAction(std::make_shared<FrightenedBTFantasma>()); 	
	
	auto filter2 = make_shared<Filter>();

	filter2->addCondition(make_shared<TimeOutBTFantasma>());
	filter2->addAction(make_shared<ScatterBTFantasma>());

	auto chase = make_shared<ChaseBTFantasma>();

	
	// Action
	root->addChild(filter);
	root->addChild(newBehavior);
	root->addChild(filter2);
	root->addChild(chase);
}

BTFantasma::~BTFantasma() {
	// TODO Auto-generated destructor stub
}

Move BTFantasma::getMove(const GameState& gs){
	InfoBTFantasma::getInfo()->in_character=character;
	InfoBTFantasma::getInfo()->in_gamestate=&gs;
	

	root->tick();


	return InfoBTFantasma::getInfo()->out_move;
}

TimeOutBTFantasma::TimeOutBTFantasma() : Behavior() {
	lastTime = std::chrono::high_resolution_clock::now();

}

Status TimeOutBTFantasma::update(){
	std::chrono::duration<float> timeStamp = std::chrono::high_resolution_clock::now() - lastTime;
	if( (int)timeStamp.count()%27 < 7){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}
}

Status GuardPowerPillCondition::update()
{
	auto gs = InfoBTFantasma::getInfo()->in_gamestate;

	if(gs->getMaze().getPowerPillPositions().size() == 1)
	{
        return BH_SUCCESS;
	}
	else
	{
        return BH_FAILURE;
	}
}

Status PowerpillBTFantasma::update(){
    auto character = InfoBTFantasma::getInfo()->in_character;
    auto ghost = dynamic_cast<Ghost*>(character.get());

    if( ghost!=nullptr && ghost->isEdible()){
        return BH_SUCCESS;
    }else{
        return BH_FAILURE;
    }

}

GuardPowerPill::GuardPowerPill() : Behavior()
{
	
}

Status GuardPowerPill::update()
{

    cout << "BTFantasma GuardPowerPill \n";
	auto gs = InfoBTFantasma::getInfo()->in_gamestate;

	auto target = gs->getMaze().getPowerPillPositions()[0];
    
	float min=1000000000;
	Move minMove=PASS;
	vector<Move> moves;
	auto character = InfoBTFantasma::getInfo()->in_character;
	
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

	InfoBTFantasma::getInfo()->out_move = minMove;
	return BH_SUCCESS;
    
}

Status ChaseBTFantasma::update(){
	cerr << "BTFantasma Chase \n" ;
	auto gs = InfoBTFantasma::getInfo()->in_gamestate;
	
	// auto pacmanPos = gs->getMaze().getNodePos(gs->getPacmanPos());

	//direccion de pacman
	auto direction = gs->getPacmanDir();
	Move m = static_cast<Move>(direction);
	Move mOpossite = PASS ;
    switch (m)
    {
    case UP:
        mOpossite = DOWN;
        break;
    case DOWN:
        mOpossite = UP;
        break;
    case LEFT:
        mOpossite = RIGHT;
        break;
    case RIGHT:
        mOpossite = LEFT;
        break;
    default:
        mOpossite = PASS;
        break;
    }

	//nodo de pacman
	int pacmanPos = gs->getPacmanPos();
	
	// Se obtienen las tres casillas atras de pacman, segun la direccion que esta mirando
	if(mOpossite != PASS)
    {
        for (int i = 0; i < 3; i++) {

            int nextNode = gs->getMaze().getNeighbour(pacmanPos, mOpossite);
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
    auto target = gs->getMaze().getNodePos(pacmanPos);
	
	float min=1000000000;
	Move minMove=PASS;
	vector<Move> moves;
	auto character = InfoBTFantasma::getInfo()->in_character;
	
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
	InfoBTFantasma::getInfo()->out_move = minMove;
	return BH_SUCCESS;
}


ScatterBTFantasma :: ScatterBTFantasma() : Behavior(){
    target = std::make_pair(104,116);

}

Status ScatterBTFantasma::update(){
	std::cerr << "BTFantasma Scatter \n" ;
    
	auto character = InfoBTFantasma::getInfo()->in_character;
	auto gs = InfoBTFantasma::getInfo()->in_gamestate;
    
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
	InfoBTFantasma::getInfo()->out_move = minMove;
	return BH_SUCCESS;
    
}


FrightenedBTFantasma::FrightenedBTFantasma() : Behavior(), e(rand()), uniform_dist(0,3){

}

Status FrightenedBTFantasma::update(){
    std::cerr << "BTFantasma Frightened \n" ;
    auto character = InfoBTFantasma::getInfo()->in_character;
    auto gs = InfoBTFantasma::getInfo()->in_gamestate;
    std::vector<Move> moves;
    if(character->getDirection()==PASS) {
        moves=gs->getMaze().getPossibleMoves(character->getPos());
    } else {
        moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
    }
    Move m = moves[rand()%moves.size()];
    InfoBTFantasma::getInfo()->out_move = m;
    return BH_SUCCESS; 
}
