#pragma once

#include "Controller.h"
#include "BehaviorTree.h"
#include <chrono>

#include <random>

using namespace std;
class InfoBTFantasma{
    static InfoBTFantasma *info;
    InfoBTFantasma(){}

public:
    static InfoBTFantasma* getInfo(){
        if(info==nullptr)info = new InfoBTFantasma();
        return info;
    }
    const GameState* in_gamestate;
    Move out_move;
    std::shared_ptr<Character> in_character;
};

class BTFantasma: public Controller {
private:
    std::shared_ptr<Composite> root;
public:
	BTFantasma(std::shared_ptr<Character> character);
	virtual ~BTFantasma();
	virtual Move getMove(const GameState& gs) override;
};

class ChaseBTFantasma : public Behavior{
public:
    virtual Status update() override;

};

class FrightenedBTFantasma : public Behavior{
private:
    std::mt19937 e;
    std::uniform_int_distribution<int> uniform_dist;
public:
    virtual Status update() override;
    FrightenedBTFantasma ();

};

class ScatterBTFantasma : public Behavior{
private:
    std::pair<int,int> target;

public:
    virtual Status update() override;
    ScatterBTFantasma();

};

class GuardPowerPill : public Behavior
{
public:
    virtual Status update() override;
    GuardPowerPill();
};

class GuardPowerPillCondition : public Behavior
{
public:
    virtual Status update() override;
};

class PowerpillBTFantasma : public Behavior{
public:
    virtual Status update() override;
};

class TimeOutBTFantasma : public Behavior{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    virtual Status update() override;
    TimeOutBTFantasma();
};

