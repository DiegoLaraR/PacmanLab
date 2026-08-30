#pragma once

#include "Controller.h"

class PacmanDTController: public Controller{

private:
    int lives;

public:
	PacmanDTController(std::shared_ptr<Character> character);

    virtual ~PacmanDTController();
    virtual Move getMove(const GameState& game) override;
};
