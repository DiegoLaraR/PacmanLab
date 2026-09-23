#pragma once

#include "Controller.h"
#include <random>
#include "FSM.h"

using namespace std;

class BlinkyStateMachine;

class BlinkyController: public Controller {
	std::mt19937 e;
	std::uniform_int_distribution<int> uniform_dist;
	std::shared_ptr<BlinkyStateMachine> fsm;
public:
	BlinkyController(std::shared_ptr<Character> character);
	virtual ~BlinkyController();
	virtual Move getMove(const GameState& game)override;
};

class NonFrigtnedTransitionBlinky:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;
	public:
	NonFrigtnedTransitionBlinky(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class FrigtnedTransitionBlinky:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;

	public:
	FrigtnedTransitionBlinky(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class CruiseElroyTransition:public FSMTransition
{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;

	public:
	CruiseElroyTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};


class TimeTransitionBlinky : public FSMTransition{
	
	float _time;
	std::chrono::high_resolution_clock::time_point _start;
	std::shared_ptr<FSMState> _next;
public:
	TimeTransitionBlinky(std::shared_ptr<FSMState> next, float time);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
	void onTransition(const GameState& gs) override;
};



class ChaseBlinky:public FSMState{

public:
	ChaseBlinky(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseBlinky();

};

class ScatterBlinky : public FSMState{

public:
	ScatterBlinky(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ScatterBlinky();

};

class CruiseElroy : public FSMState
{

public:
	CruiseElroy(shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~CruiseElroy();
};


class NonfrigtnedBlinky : public FSMState
{
public:
	NonfrigtnedBlinky(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~NonfrigtnedBlinky();

};

class FrigtnedBlinky : public FSMState
{
public:
	FrigtnedBlinky(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~FrigtnedBlinky();

};

class BlinkyStateMachine: public FiniteStateMachine{

public:
	BlinkyStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~BlinkyStateMachine();

};