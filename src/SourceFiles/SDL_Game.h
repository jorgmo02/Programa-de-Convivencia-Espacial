#pragma once
#include <SDL.h>
#include "Constants.h"
#include <memory>
#include <assert.h>

#include "EntityManager.h"
#include "SDLTexturesManager.h"
#include "SDLAudioManager.h"
#include "SDLFontsManager.h"
#include "GameStateMachine.h"

#include "SDL_macros.h"
#include "iostream"
#include "checkML.h"


using namespace std;
//Interfaz entre el juego y el display
class InputHandler;
class Constants;
#define CONST(type,key) SDL_Game::instance()->getConstants()->getConstant<type>(key)
class SDL_Game
{
protected:

	SDLFontsManager* fonts_ = nullptr;
	SDLTexturesManager* textures_ = nullptr;
	SDLAudioManager* audio_ = nullptr;
	GameStateMachine* gamestateMachine_ = nullptr;
	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	InputHandler* inputHandler_ =  nullptr;
	Constants constants_;

	static unique_ptr<SDL_Game> instance_;
private:
	bool exit_;
	void initializeResources();
	void closeResources();

public:
	void start();
	SDL_Game();
	virtual ~SDL_Game();

	SDL_Game(SDL_Game&) = delete;
	SDL_Game& operator=(SDL_Game&) = delete;

	/*inline static SDL_Game* instance() {
		assert(instance_.get() != nullptr);
		return instance_.get();
		std::cout << "He sido llamado por singleton" << std::endl;
	}*/

	inline static SDL_Game* instance() {
		if (instance_.get() == nullptr) {
			instance_.reset(new SDL_Game());
		}
		return instance_.get();
	}


	inline unsigned int getTime() {
		return SDL_GetTicks();
	}

	InputHandler* getInputHandler() { return inputHandler_; }

	SDLTexturesManager* getTexturesMngr() { return textures_; }
	SDLAudioManager* getAudioMngr() { return audio_; }
	SDLFontsManager* getFontMngr() { return fonts_; }
	GameStateMachine* getStateMachine() { return gamestateMachine_; }
	SDL_Renderer* getRenderer() { return  renderer_; }
	const Constants* getConstants() { return &constants_; }
};
