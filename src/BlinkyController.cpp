#include "BlinkyController.h"

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

BlinkyController::BlinkyController(std::shared_ptr<Character> character):
	Controller(character),
	e(rand()),
	uniform_dist(0,3),
	fsm(std::make_shared<BlinkyStateMachine>(character)) {
}

BlinkyController::~BlinkyController() {
	// TODO Auto-generated destructor stub
}

Move 
BlinkyController::getMove(const GameState& game){
	return fsm->update(game);

}

///////////////////TimeTransitionBlinky//////////////
TimeTransitionBlinky::TimeTransitionBlinky(std::shared_ptr<FSMState> next, float time): _next(next), _time(time)
{
	_start = std::chrono::high_resolution_clock::now();
}

bool TimeTransitionBlinky::isValid(const GameState& gs)
{
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = now - _start;

	return elapsed.count() >= _time;
}

std::shared_ptr<FSMState> TimeTransitionBlinky::getNextState()
{
	return _next;
}

void TimeTransitionBlinky::onTransition(const GameState& gs)
{
	_start = std::chrono::high_resolution_clock::now();
}


///////////////////////////////////NonFrigtnedTransitionBlinky///////////////////////////////
NonFrigtnedTransitionBlinky::NonFrigtnedTransitionBlinky(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character):_next(next), _character(character){
}
bool NonFrigtnedTransitionBlinky::isValid(const GameState& gs){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(!ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> NonFrigtnedTransitionBlinky::getNextState(){
	return _next;
}


//////////////////////////////////CruiseElroyTransition///////////////////

CruiseElroyTransition::CruiseElroyTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character): _next(next), _character(character){
}

bool CruiseElroyTransition::isValid(const GameState& gs)
{
	auto pills = gs.getMaze().getPillPositions().size();

	if(pills <= (gs.getMaze().getTotalPill() / 3))
	{
		return true;
	}
	return false;
}
shared_ptr<FSMState> CruiseElroyTransition::getNextState()
{
	return _next;
}


//////////////////////////////////FrigtnedTransitionBlinky///////////////////
FrigtnedTransitionBlinky::FrigtnedTransitionBlinky(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character): _next(next), _character(character){
	
}

bool FrigtnedTransitionBlinky::isValid(const GameState& gs){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> FrigtnedTransitionBlinky::getNextState(){
	return _next;
}




/////////////////////////////////Estados/////////////////////////////////////////


///////////////////////////////ChaseBlinky///////////////////////////////////////
ChaseBlinky::ChaseBlinky(std::shared_ptr<Character> _character):FSMState(_character){
	
}
void ChaseBlinky::onEnter(const GameState& ){
	
	std::dynamic_pointer_cast<Ghost>(character)->revert(); 
}
Move ChaseBlinky::onUpdate(const GameState& game){
	std::vector<Move> moves;

	//std::cout << "chase\n";
	const auto pacmanCoord=game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int minI=0;
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			pacmanCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return moves[minI];
}
ChaseBlinky::~ChaseBlinky(){}


// ScatterState
ScatterBlinky::ScatterBlinky(std::shared_ptr<Character> _character):FSMState(_character){}

void ScatterBlinky::onEnter(const GameState&)
{
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move ScatterBlinky::onUpdate(const GameState& game){

	//std::cout << "Scatter\n";
	std::vector<Move> moves;
	const auto myPos=character->getPos();
	const std::pair<int, int> corner = {104, 4};
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			corner);
	int minI=0;
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			corner);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return moves[minI];
}
ScatterBlinky::~ScatterBlinky(){}


///////////////////////////FrigtnedBlinky/////////////////////////// 
FrigtnedBlinky::FrigtnedBlinky(std::shared_ptr<Character> _character) : FSMState(_character){}

void FrigtnedBlinky::onEnter(const GameState&)
{}

Move FrigtnedBlinky ::onUpdate(const GameState& game){
	std::vector<Move> moves;

	//std::cout << "Frigtned\n";
	const auto pacmanCoord = game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos = character->getPos();

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	// Obtiene la distancia euclidiana entre Pacman y Blinky
	float max=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int maxI=0;

	//Revisa todos los movimientos legales y devuelve el que aleja mas de pacman
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			pacmanCoord);
		if(dist>max){
			max=dist;
			maxI=i;
		}
	}
	return moves[maxI];
}
FrigtnedBlinky::~FrigtnedBlinky(){

}



/////////////////////////////////////BlinkyStateMachine/////////////////////////////
BlinkyStateMachine::BlinkyStateMachine(std::shared_ptr<Character> _character):FiniteStateMachine(_character){
	
	auto chase = std::make_shared<ChaseBlinky>(character);
	auto frigtned = std::make_shared<FrigtnedBlinky>(character);
	auto scatter = std::make_shared<ScatterBlinky>(character);

	scatter->addTransition(std::make_shared<FrigtnedTransitionBlinky>(frigtned, character));
	chase->addTransition(std::make_shared<FrigtnedTransitionBlinky>(frigtned, character));
	
	scatter->addTransition(make_shared<CruiseElroyTransition>(chase, character));
	
	scatter->addTransition(std::make_shared<TimeTransitionBlinky>(chase, 7.0f));
	chase->addTransition(std::make_shared<TimeTransitionBlinky>(scatter, 20.0f));
	

	frigtned->addTransition(std::make_shared<NonFrigtnedTransitionBlinky>(chase, character));
	
	states.push_back(chase);
	states.push_back(frigtned);
	states.push_back(scatter);
	
	initialState = scatter;
	activeState	= initialState;	
}



Move BlinkyStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}


BlinkyStateMachine::~BlinkyStateMachine(){

}