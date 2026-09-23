#pragma once

#include "Controller.h"
#include <random>
#include "FSM.h"

using namespace std;

class FSMFantasmaStateMachine;

class FSMFantasma: public Controller {
	std::mt19937 e;
	std::uniform_int_distribution<int> uniform_dist;
	std::shared_ptr<FSMFantasmaStateMachine> fsm;
public:
	FSMFantasma(std::shared_ptr<Character> character);
	virtual ~FSMFantasma();
	virtual Move getMove(const GameState& game)override;
};

class NonFrigtnedTransitionFSMFantasma:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;
	public:
	NonFrigtnedTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class FrigtnedTransitionFSMFantasma:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;

	public:
	FrigtnedTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};


class ChaseTransitionFSMFantasma : public FSMTransition
{

private:
	shared_ptr<FSMState> _next;
	shared_ptr<Character> _character;
	float _distance;
public:

	ChaseTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character, float _distance);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class ScatterTransitionFSMFantasma : public FSMTransition
{

private:
	shared_ptr<FSMState> _next;
	shared_ptr<Character> _character;
	float _distance;
public:

	ScatterTransitionFSMFantasma(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character, float _distance);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class ChaseFSMFantasma:public FSMState{

public:
	ChaseFSMFantasma(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseFSMFantasma();

};

class ScatterFSMFantasma : public FSMState{
private:

    vector<pair<int, int>> patrolPoints;
	int currentPoint;

public:
	ScatterFSMFantasma(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ScatterFSMFantasma();

};

class NonfrigtnedFSMFantasma : public FSMState
{
public:
	NonfrigtnedFSMFantasma(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~NonfrigtnedFSMFantasma();

};

class FrigtnedFSMFantasma : public FSMState
{
public:
	FrigtnedFSMFantasma(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~FrigtnedFSMFantasma();

};

class FSMFantasmaStateMachine: public FiniteStateMachine{

public:
	FSMFantasmaStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~FSMFantasmaStateMachine();

};