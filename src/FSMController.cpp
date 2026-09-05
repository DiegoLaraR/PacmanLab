/*
 * FSMController.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#include "FSMController.h"
#include <iostream>
#include <chrono>
#include <thread>

FSMController::FSMController(std::shared_ptr<Character> character):
	Controller(character),
	e(rand()),
	uniform_dist(0,3),
	fsm(std::make_shared<ExampleStateMachine>(character)) {
}

FSMController::~FSMController() {
	// TODO Auto-generated destructor stub
}

Move 
FSMController::getMove(const GameState& game){
	return fsm->update(game);
}


///////////////////////////////////PillTransition///////////////////////////////
PillTransition::PillTransition(std::shared_ptr<FSMState> next):last(0),_next(next){

}

bool PillTransition::isValid(const GameState& gs){
	int quedan=gs.getMaze().getPillPositions().size();
	if(last!=quedan && quedan%20==0){
		last =quedan;
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> PillTransition::getNextState(){
	return _next;
}



///////////////////TimeTransition//////////////
TimeTransition::TimeTransition(std::shared_ptr<FSMState> next, float time): _next(next), _time(time)
{
	_start = std::chrono::high_resolution_clock::now();
}

bool TimeTransition::isValid(const GameState& gs)
{
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = now - _start;

	return elapsed.count() >= _time;
}

std::shared_ptr<FSMState> TimeTransition::getNextState()
{
	return _next;
}

void TimeTransition::onTransition(const GameState& gs)
{
	_start = std::chrono::high_resolution_clock::now();
}


///////////////////////////////////NonFrigtnedTransition///////////////////////////////
NonFrigtnedTransition::NonFrigtnedTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character):_next(next), _character(character){
}
bool NonFrigtnedTransition::isValid(const GameState& gs){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(!ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> NonFrigtnedTransition::getNextState(){
	return _next;
}

//////////////////////////////////FrigtedTransition///////////////////
FrigtnedTransition::FrigtnedTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character): _next(next), _character(character){
	
}

bool FrigtnedTransition::isValid(const GameState& gs){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> FrigtnedTransition::getNextState(){
	return _next;
}









/////////////////////////////////Estados/////////////////////////////////////////
///////////////////////////////ChaseState///////////////////////////////////////
ChaseState::ChaseState(std::shared_ptr<Character> _character):FSMState(_character){
	
}
void ChaseState::onEnter(const GameState& ){
	
	std::dynamic_pointer_cast<Ghost>(character)->revert(); 
}
Move ChaseState::onUpdate(const GameState& game){
	std::vector<Move> moves;

	std::cout << "chase\n";
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
ChaseState::~ChaseState(){}


// ScatterState
ScatterState::ScatterState(std::shared_ptr<Character> _character):FSMState(_character){}

void ScatterState::onEnter(const GameState&)
{
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move ScatterState::onUpdate(const GameState& game){

	std::cout << "Scatter\n";
	std::vector<Move> moves;
	const auto myPos=character->getPos();
	const std::pair<int, int> corner = {-20, -20};
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
ScatterState::~ScatterState(){}


///////////////////////////FrigtnedState/////////////////////////// 
FrigtnedState::FrigtnedState(std::shared_ptr<Character> _character) : FSMState(_character){}

void FrigtnedState::onEnter(const GameState&)
{}

Move FrigtnedState ::onUpdate(const GameState& game){
	std::vector<Move> moves;

	std::cout << "Frigtned\n";
	const auto pacmanCoord = game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos = character->getPos();

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float max=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int maxI=0;
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
FrigtnedState::~FrigtnedState(){

}



/////////////////////////////////////BlinkyStateMachine/////////////////////////////
ExampleStateMachine::ExampleStateMachine(std::shared_ptr<Character> _character):FiniteStateMachine(_character){
	
	auto chase = std::make_shared<ChaseState>(character);
	auto frigtned = std::make_shared<FrigtnedState>(character);
	auto scatter = std::make_shared<ScatterState>(character);

	scatter->addTransition(std::make_shared<FrigtnedTransition>(frigtned, character));
	chase->addTransition(std::make_shared<FrigtnedTransition>(frigtned, character));
	
	scatter->addTransition(std::make_shared<TimeTransition>(chase, 7.0f));
	chase->addTransition(std::make_shared<TimeTransition>(scatter, 20.0f));
	
	frigtned->addTransition(std::make_shared<NonFrigtnedTransition>(chase, character));
	
	states.push_back(chase);
	states.push_back(frigtned);
	states.push_back(scatter);
	
	initialState = scatter;
	activeState	= initialState;
	
}



Move ExampleStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}


ExampleStateMachine::~ExampleStateMachine(){

}