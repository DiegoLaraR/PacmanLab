#pragma once

#include "Controller.h"
#include "BehaviorTree.h"


class InfoPacman{
    static InfoPacman *info;
    InfoPacman(){}

public:
    static InfoPacman* getInfo(){
        if(info==nullptr)info = new InfoPacman();
        return info;
    }
    const GameState* in_gamestate;
    Move out_move;
    std::shared_ptr<Character> in_character;
};

class PacmanDTController: public Controller{
private:
    std::shared_ptr<Composite> root;

public:
	PacmanDTController(std::shared_ptr<Character> character);
    virtual ~PacmanDTController();
    virtual Move getMove(const GameState& gs) override;
};


class FrigtnedPacman : public Behavior
{
public:

    virtual Status update() override;
    FrigtnedPacman();
};
class EatPillPacman : public Behavior
{
public:

    virtual Status update() override;
    EatPillPacman();

};
class EatPowerPillPacman : public Behavior
{
public:

    virtual Status update() override;
    EatPowerPillPacman();
};

class EatGhostPacman : public Behavior
{
public:

    virtual Status update() override;
    EatGhostPacman();
};


// Transiciones

class FrigtnedTransitionPacman : public Behavior
{
private:
    bool isFleeing = false;
public:
    FrigtnedTransitionPacman();
    virtual Status update() override;
};

class PowerPillTransition : public Behavior
{
public:
    PowerPillTransition();
    virtual Status update() override;
};

class EatGhostTransition : public Behavior
{
public:

    EatGhostTransition();
    virtual Status update() override;
};
