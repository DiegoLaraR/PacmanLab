#include "FSMFantasma.h"

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

FSMFantasma::FSMFantasma(std::shared_ptr<Character> character):
	Controller(character),
	e(rand()),
	uniform_dist(0,3),
	fsm(std::make_shared<FSMFantasmaStateMachine>(character)) {
}

FSMFantasma::~FSMFantasma() {
	// TODO Auto-generated destructor stub
}

Move 
FSMFantasma::getMove(const GameState& game){
	return fsm->update(game);

}

///////////////////////////////////NonFrigtnedTransitionFSMFantasma///////////////////////////////
NonFrigtnedTransitionFSMFantasma::NonFrigtnedTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character):_next(next), _character(character){
}
bool NonFrigtnedTransitionFSMFantasma::isValid(const GameState&){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(!ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> NonFrigtnedTransitionFSMFantasma::getNextState(){
	return _next;
}



//////////////////////////////////FrigtnedTransitionFSMFantasma///////////////////
FrigtnedTransitionFSMFantasma::FrigtnedTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character): _next(next), _character(character){
	
}

bool FrigtnedTransitionFSMFantasma::isValid(const GameState&){
	
	Ghost *ghost = dynamic_cast<Ghost*>(_character.get());
	if(ghost->isEdible())
	{
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> FrigtnedTransitionFSMFantasma::getNextState(){
	return _next;
}


//////////////////////////////////ChaseTransitionFSMFantasma///////////////////

ChaseTransitionFSMFantasma::ChaseTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character, float distance) 
: _next(next), _character(character), _distance(distance){
}

bool ChaseTransitionFSMFantasma::isValid(const GameState& gs){
	
	auto pacmanPos = gs.getMaze().getNodePos(gs.getPacmanPos());
	auto suePos = gs.getMaze().getNodePos(_character->getPos());

	auto dist = euclid2(suePos, pacmanPos);

	return dist < _distance;
}
std::shared_ptr<FSMState> ChaseTransitionFSMFantasma::getNextState(){
	return _next;
}

//////////////////////////////////ScatterTransitionFSMFantasma///////////////////

ScatterTransitionFSMFantasma::ScatterTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character, float distance) : _next(next), _character(character), _distance(distance){
}

bool ScatterTransitionFSMFantasma::isValid(const GameState& gs){
	
	auto pacmanPos = gs.getMaze().getNodePos(gs.getPacmanPos());
	auto suePos = gs.getMaze().getNodePos(_character->getPos());

	auto dist = euclid2(suePos, pacmanPos);

	return dist > _distance;
}
std::shared_ptr<FSMState> ScatterTransitionFSMFantasma::getNextState(){
	return _next;
}

/////////////////////////////////Estados/////////////////////////////////////////
///////////////////////////////ChaseFSMFantasma///////////////////////////////////////
ChaseFSMFantasma::ChaseFSMFantasma(std::shared_ptr<Character> _character):FSMState(_character){
	
}
void ChaseFSMFantasma::onEnter(const GameState& ){
	
	std::dynamic_pointer_cast<Ghost>(character)->revert(); 
}
Move ChaseFSMFantasma::onUpdate(const GameState& game){
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
ChaseFSMFantasma::~ChaseFSMFantasma(){}


// ScatterState
ScatterFSMFantasma::ScatterFSMFantasma(std::shared_ptr<Character> _character):FSMState(_character)
{

    patrolPoints.push_back({4, 4});
	patrolPoints.push_back({104, 4});
	patrolPoints.push_back({104, 116});
	patrolPoints.push_back({4, 116});

}

void ScatterFSMFantasma::onEnter(const GameState&)
{
	std::dynamic_pointer_cast<Ghost>(character)->revert();
    currentPoint = 0;
}

Move ScatterFSMFantasma::onUpdate(const GameState& game){

	std::cout << "FSMFantasmas Scatter\n";
	std::vector<Move> moves;

	const auto myPos=character->getPos();
	
	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	auto target = patrolPoints[currentPoint];

	auto currentPos = game.getMaze().getNodePos(myPos);

	float distance = euclid2(currentPos, target);

	if(distance < 25)
	{
		currentPoint++;

		if(currentPoint >= (int)patrolPoints.size())
		{
			currentPoint = 0;
		}
		target = patrolPoints[currentPoint];
	}
	
	float min=100000000;
	int minI=0;

	for(unsigned int i=0 ; i < moves.size() ; i++){
		
		if(moves[i] == PASS)
			continue;
		
		int nextNode= game.getMaze().getNeighbour(myPos, moves[i]);

		if(nextNode == -1)
			continue;

		auto nextPos = game.getMaze().getNodePos(nextNode);
		auto dist = euclid2(nextPos, target);

		if(dist < min)
		{
			min = dist;
			minI = i;
		}
	}
	return moves[minI];
}
ScatterFSMFantasma::~ScatterFSMFantasma(){}


///////////////////////////FrigtnedFSMFantasma/////////////////////////// 




///////////////////////////FrigtnedFSMFantasma/////////////////////////// 
FrigtnedFSMFantasma::FrigtnedFSMFantasma(std::shared_ptr<Character> _character) : FSMState(_character){}

void FrigtnedFSMFantasma::onEnter(const GameState&)
{}

Move FrigtnedFSMFantasma ::onUpdate(const GameState& game){
	std::vector<Move> moves;

	//std::cout << "Frigtned\n";
	const auto pacmanCoord = game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos = character->getPos();

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	// Obtiene la distancia euclidiana entre Pacman y FSMFantasma
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
FrigtnedFSMFantasma::~FrigtnedFSMFantasma(){

}



/////////////////////////////////////FSMFantasmaStateMachine/////////////////////////////
FSMFantasmaStateMachine::FSMFantasmaStateMachine(std::shared_ptr<Character> _character):FiniteStateMachine(_character){
	
	auto chase = std::make_shared<ChaseFSMFantasma>(character);
	auto frigtned = std::make_shared<FrigtnedFSMFantasma>(character);
	auto scatter = std::make_shared<ScatterFSMFantasma>(character);

	scatter->addTransition(std::make_shared<FrigtnedTransitionFSMFantasma>(frigtned, character));
	chase->addTransition(std::make_shared<FrigtnedTransitionFSMFantasma>(frigtned, character));

	scatter->addTransition(std::make_shared<ChaseTransitionFSMFantasma>(chase, character, 49));
	chase->addTransition(std::make_shared<ScatterTransitionFSMFantasma>(scatter, character, 49));
	

	frigtned->addTransition(std::make_shared<NonFrigtnedTransitionFSMFantasma>(chase, character));
	
	states.push_back(chase);
	states.push_back(frigtned);
	states.push_back(scatter);
	
	initialState = scatter;
	activeState	= initialState;	
}



Move FSMFantasmaStateMachine::update(const GameState& gs){
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}
	return activeState->onUpdate(gs);
}


FSMFantasmaStateMachine::~FSMFantasmaStateMachine(){

}