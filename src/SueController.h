#pragma once

#pragma once

#include "Controller.h"
#include <random>
#include "FSM.h"

using namespace std;

class SueStateMachine;

class SueController: public Controller {
	std::mt19937 e;
	std::uniform_int_distribution<int> uniform_dist;
	std::shared_ptr<SueStateMachine> fsm;
public:
	SueController(std::shared_ptr<Character> character);
	virtual ~SueController();
	virtual Move getMove(const GameState& game)override;
};

class PillTransitionSue:public FSMTransition{
	int last;
	std::shared_ptr<FSMState> _next;
public:
	PillTransitionSue(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};


class NonFrigtnedTransitionSue:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;
	public:
	NonFrigtnedTransitionSue(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class FrigtnedTransitionSue:public FSMTransition{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;

	public:
	FrigtnedTransitionSue(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class TimeTransitionSue : public FSMTransition{
	
	std::shared_ptr<FSMState> _next;
	float _time;
	std::chrono::high_resolution_clock::time_point _start;
public:
	TimeTransitionSue(std::shared_ptr<FSMState> next, float time);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
	void onTransition(const GameState& gs) override;
};

class HomeTransition : public FSMTransition
{
	std::shared_ptr<FSMState> _next;
	std::shared_ptr<Character> _character;

	public:
	HomeTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> _character);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};


class ChaseSue:public FSMState{

public:
	ChaseSue(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseSue();

};

class ScatterSue : public FSMState{
	
public:
	ScatterSue(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ScatterSue();
	
};

class FrigtnedSue : public FSMState
{
	public:
	FrigtnedSue(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~FrigtnedSue();
	
};


class HomeSue : public FSMState
{
	public:
	HomeSue(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	~HomeSue();
	
};

class SueStateMachine: public FiniteStateMachine{
	
public:
	SueStateMachine(std::shared_ptr<Character> _character);
	Move update(const GameState& gs) override;
	~SueStateMachine();

};



