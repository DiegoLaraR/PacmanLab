#include "SueController.h"

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

SueController::SueController(std::shared_ptr<Character> character):
	Controller(character),
	e(rand()),
	uniform_dist(0,3),
	fsm(std::make_shared<SueStateMachine>(character)) {
}

SueController::~SueController() {
	// TODO Auto-generated destructor stub
}

Move 
SueController::getMove(const GameState& game){
	return fsm->update(game);

}



///////////////////////////////////PillTransitionSue///////////////////////////////
PillTransitionSue::PillTransitionSue(std::shared_ptr<FSMState> next):last(0),_next(next){

}

bool PillTransitionSue::isValid(const GameState& gs){
	int quedan=gs.getMaze().getPillPositions().size();
	if(last!=quedan && quedan%20==0){
		last =quedan;
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> PillTransitionSue::getNextState(){
	return _next;
}



///////////////////TimeTransitionSue//////////////
TimeTransitionSue::TimeTransitionSue(std::shared_ptr<FSMState> next, float time): _next(next), _time(time)
{
	_start = std::chrono::high_resolution_clock::now();
}

bool TimeTransitionSue::isValid(const GameState&)
{
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = now - _start;

	return elapsed.count() >= _time;
}

std::shared_ptr<FSMState> TimeTransitionSue::getNextState()
{
	return _next;
}

void TimeTransitionSue::onTransition(const GameState&)
{
	_start = std::chrono::high_resolution_clock::now();
}


///////////////////////////////////NonFrigtnedTransitionSue///////////////////////////////
NonFrigtnedTransitionSue::NonFrigtnedTransitionSue(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character):_next(next), _character(character){
}
bool NonFrigtnedTransitionSue::isValid(const GameState&){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(!ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> NonFrigtnedTransitionSue::getNextState(){
	return _next;
}


//////////////////////////////////FrigtnedTransitionSue///////////////////
FrigtnedTransitionSue::FrigtnedTransitionSue(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character): _next(next), _character(character){
	
}

bool FrigtnedTransitionSue::isValid(const GameState&){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> FrigtnedTransitionSue::getNextState(){
	return _next;
}

//////////////////////////////////HomeTransition///////////////////

HomeTransition::HomeTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character): _next(next), _character(character){
}

bool HomeTransition::isValid(const GameState& gs){
	
	auto pills = gs.getMaze().getPillPositions().size();
	
	if(pills <= static_cast<size_t>(gs.getMaze().getTotalPill() / 3))
	{
		return true;
	}
	
	return false;
	
}
std::shared_ptr<FSMState> HomeTransition::getNextState(){
	return _next;
}

//////////////////////////////////PatrolTransition///////////////////

// PatrolTransition::PatrolTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character, float distance) : _next(next), _character(character), _distance(distance){
// }

// bool PatrolTransition::isValid(const GameState& gs){
	
// 	auto pacmanPos = gs.getMaze().getNodePos(gs.getPacmanPos());
// 	auto suePos = gs.getMaze().getNodePos(_character->getPos());

// 	auto dist = euclid2(suePos, pacmanPos);

// 	return dist > _distance;
// }
// std::shared_ptr<FSMState> PatrolTransition::getNextState(){
// 	return _next;
// }

//////////////////////////////////ChaseTransition///////////////////

// ChaseTransition::ChaseTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character, float distance) : _next(next), _character(character), _distance(distance){
// }

// bool ChaseTransition::isValid(const GameState& gs){
	
// 	auto pacmanPos = gs.getMaze().getNodePos(gs.getPacmanPos());
// 	auto suePos = gs.getMaze().getNodePos(_character->getPos());

// 	auto dist = euclid2(suePos, pacmanPos);

// 	return dist < _distance;
// }
// std::shared_ptr<FSMState> ChaseTransition::getNextState(){
// 	return _next;
// }

/////////////////////////////////Estados/////////////////////////////////////////

///////////////////////////////PatrolSue///////////////////////////////////////

// PatrolSue::PatrolSue(std::shared_ptr<Character> _character):FSMState(_character), currentPoint(0){
	
// 	patrolPoints.push_back({4, 4});
// 	patrolPoints.push_back({104, 4});
// 	patrolPoints.push_back({104, 116});
// 	patrolPoints.push_back({4, 116});


// }
// void PatrolSue::onEnter(const GameState& ){
	
// 	currentPoint = 0;
// }
// Move PatrolSue::onUpdate(const GameState& game){
// 	std::vector<Move> moves;

// 	const auto myPos=character->getPos();
	
// 	if(character->getDirection()==PASS){
// 		moves=game.getMaze().getPossibleMoves(myPos);
// 	}else{
// 		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
// 	}

// 	auto target = patrolPoints[currentPoint];

// 	auto currentPos = game.getMaze().getNodePos(myPos);

// 	float distance = euclid2(currentPos, target);

// 	if(distance < 25)
// 	{
// 		currentPoint++;

// 		if(currentPoint >= (int)patrolPoints.size())
// 		{
// 			currentPoint = 0;
// 		}
// 		target = patrolPoints[currentPoint];
// 	}
	
// 	float min=100000000;
// 	int minI=0;

// 	for(unsigned int i=0 ; i < moves.size() ; i++){
		
// 		if(moves[i] == PASS)
// 			continue;
		
// 		int nextNode= game.getMaze().getNeighbour(myPos, moves[i]);

// 		if(nextNode == -1)
// 			continue;

// 		auto nextPos = game.getMaze().getNodePos(nextNode);
// 		auto dist = euclid2(nextPos, target);

// 		if(dist < min)
// 		{
// 			min = dist;
// 			minI = i;
// 		}
// 	}
// 	return moves[minI];

// }
// PatrolSue::~PatrolSue(){}



///////////////////////////////ChaseSue///////////////////////////////////////
ChaseSue::ChaseSue(std::shared_ptr<Character> _character):FSMState(_character){
	
}
void ChaseSue::onEnter(const GameState& ){
	
	std::dynamic_pointer_cast<Ghost>(character)->revert(); 
}
Move ChaseSue::onUpdate(const GameState& game){
	std::vector<Move> moves;

	//std::cout << "chase\n";
	const auto pacmanCoord=game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);
	const std::pair<int, int> corner = {-20, -20};
	
	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	if(euclid2(game.getMaze().getNodePos(myPos), pacmanCoord) <= 8)
	{

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
	else
	{
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

}
ChaseSue::~ChaseSue(){}


// ScatterState
ScatterSue::ScatterSue(std::shared_ptr<Character> _character):FSMState(_character){}

void ScatterSue::onEnter(const GameState&)
{
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}

Move ScatterSue::onUpdate(const GameState& game){

	//std::cout << "Scatter\n";
	std::vector<Move> moves;
	const auto myPos=character->getPos();
	const std::pair<int, int> corner = {4, 116};
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
ScatterSue::~ScatterSue(){}


///////////////////////////HomeSue/////////////////////////// 

HomeSue::HomeSue(std::shared_ptr<Character> _character) : FSMState(_character){}

Move HomeSue ::onUpdate(const GameState&){

	return PASS;
}
HomeSue::~HomeSue(){

}



///////////////////////////FrigtnedSue/////////////////////////// 
FrigtnedSue::FrigtnedSue(std::shared_ptr<Character> _character) : FSMState(_character){}

void FrigtnedSue::onEnter(const GameState&)
{}

Move FrigtnedSue ::onUpdate(const GameState& game){
	std::vector<Move> moves;

	//std::cout << "Frigtned\n";
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
FrigtnedSue::~FrigtnedSue(){

}



/////////////////////////////////////SueStateMachine/////////////////////////////
SueStateMachine::SueStateMachine(std::shared_ptr<Character> _character):FiniteStateMachine(_character){
	
	auto chase = std::make_shared<ChaseSue>(character);
	auto frigtned = std::make_shared<FrigtnedSue>(character);
	auto scatter = std::make_shared<ScatterSue>(character);
	auto home = std::make_shared<HomeSue>(character);
	//auto patrol = std::make_shared<PatrolSue>(character);

	home->addTransition(make_shared<HomeTransition>(chase, character));

	scatter->addTransition(std::make_shared<FrigtnedTransitionSue>(frigtned, character));
	chase->addTransition(std::make_shared<FrigtnedTransitionSue>(frigtned, character));
	
	scatter->addTransition(std::make_shared<TimeTransitionSue>(chase, 7.0f));
	chase->addTransition(std::make_shared<TimeTransitionSue>(scatter, 20.0f));
	
	//chase->addTransition(make_shared<PatrolTransition>(patrol, character, 50.0f));

	frigtned->addTransition(std::make_shared<NonFrigtnedTransitionSue>(chase, character));
	
	//patrol->addTransition(make_shared<ChaseTransition>(chase, character, 30.0f));

	states.push_back(home);
	states.push_back(chase);
	states.push_back(frigtned);
	states.push_back(scatter);
	// states.push_back(patrol);
	
	initialState = home;
	activeState	= initialState;	
}



Move SueStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}


SueStateMachine::~SueStateMachine(){

}