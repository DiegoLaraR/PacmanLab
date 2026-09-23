#pragma once

#include "Controller.h"
#include "BehaviorTree.h"
#include <chrono>

#include <random>
class InfoPinky{
    static InfoPinky *info;
    InfoPinky(){}

public:
    static InfoPinky* getInfo(){
        if(info==nullptr)info = new InfoPinky();
        return info;
    }
    const GameState* in_gamestate;
    Move out_move;
    std::shared_ptr<Character> in_character;
};

class PinkyController: public Controller {
private:
    std::shared_ptr<Composite> root;
public:
	PinkyController(std::shared_ptr<Character> character);
	virtual ~PinkyController();
	virtual Move getMove(const GameState& gs) override;
};

class ChaseStatePinky : public Behavior{
public:
    virtual Status update() override;

};

class FrightenedStatePinky : public Behavior{
private:
    std::mt19937 e;
    std::uniform_int_distribution<int> uniform_dist;
public:
    virtual Status update() override;
    FrightenedStatePinky ();

};

class ScatterStatePinky : public Behavior{
private:
    std::pair<int,int> target;

public:
    virtual Status update() override;
    ScatterStatePinky();

};

class PowerpillTransition : public Behavior{
public:
    virtual Status update() override;
};

class TimeOutTransition : public Behavior{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    virtual Status update() override;
    TimeOutTransition();
};
