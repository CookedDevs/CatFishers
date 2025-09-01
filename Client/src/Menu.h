#pragma once
#include "State.h"

class Menu : public State
{
public:
	Menu() {}
	~Menu() {}

	void UnInit() override;
	void Init() override;
	void Update() override;
	std::string GetName() override;
private:
	int perlinNoiseMapLoc;
	int timeLoc;
};



