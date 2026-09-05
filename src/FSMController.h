/*
 * FSMController.h
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#ifndef FSMCONTROLLER_H_
#define FSMCONTROLLER_H_

#include "Controller.h"
#include <random>
#include "FSM.h"

class ExampleStateMachine;

class FSMController: public Controller {
	std::mt19937 e;
	std::uniform_int_distribution<int> uniform_dist;
	std::shared_ptr<ExampleStateMachine> fsm;
public:
	FSMController(std::shared_ptr<Character> character);
	virtual ~FSMController();
	virtual Move getMove(const GameState& game)override;
};

class PillTransition:public FSMTransition{
	int last;
	std::shared_ptr<FSMState> _next;
public:
	PillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};


class NonFrigtnedTransition:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;
	public:
	NonFrigtnedTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class FrigtnedTransition:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;

	public:
	FrigtnedTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};



class TimeTransition : public FSMTransition{
	
	float _time;
	std::chrono::high_resolution_clock::time_point _start;
	std::shared_ptr<FSMState> _next;
public:
	TimeTransition(std::shared_ptr<FSMState> next, float time);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
	void onTransition(const GameState& gs) override;
};



class ChaseState:public FSMState{

public:
	ChaseState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseState();

};

class ScatterState : public FSMState{

public:
	ScatterState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ScatterState();

};


class Nonfrigtned : public FSMState
{
public:
	Nonfrigtned(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~Nonfrigtned();

};

class FrigtnedState : public FSMState
{
public:
	FrigtnedState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~FrigtnedState();

};

class ExampleStateMachine: public FiniteStateMachine{

public:
	ExampleStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~ExampleStateMachine();

};
#endif