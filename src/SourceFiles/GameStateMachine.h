#pragma once
#include <vector>
#include <string>
#include "GameState.h"
#include "checkML.h"
#include "Texture.h"

class GameStateMachine
{
private:
	std::vector<GameState*> states_;

	int currentState_ = -1;

	void update();
	void render();
	void handleInput();
public:
	GameStateMachine();
	virtual ~GameStateMachine();
	/*Cambia al estado pausa con un owner*/
	void setPauseOwner(int ownerID);

	void changeToState(int state, int numberOfPlayers = 1, int gameMode = 0, std::string tileMap = "");
	void transitionToState(int state, int numberOfPlayers = 1, int gameMode = 0, std::string tileMap = "");
	void loadState(int state, int numberOfPlayers, int gameMode, string tileMap);
	GameState* getCurrentState() const { return ((currentState_ != -1) ? states_[currentState_] : nullptr); };
	void deleteState(int state);

	void gameCycle();
};

