#pragma once
#include "State.h"

class Game : public State
{
public:
	Game() {}
	~Game();

	void Init() override;
	void Update() override;
	std::string GetName() override;
private:

};