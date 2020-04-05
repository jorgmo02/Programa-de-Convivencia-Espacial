#include "ControllerGameMode.h"
#include <map>

ControllerGameMode::~ControllerGameMode()
{

}

void ControllerGameMode::init(PlayState* game)  {
	GameMode::init(game);
	for (int i = 0; i < nPlayers_; i++) {
		players_.push_back(PlayerFactory::createPlayerWithHealth(game->getEntityManager(), game->getPhysicsWorld(), i, 
			Resources::Tinky, tilemap_->getPlayerSpawnPoint(i).x, tilemap_->getPlayerSpawnPoint(i).y, 3));
	}
	controller_ = ObjectFactory::makeController(state_->getEntityManager(), state_->getPhysicsWorld(), b2Vec2(tilemap_->getObjSpecialSpawnPos().x, tilemap_->getObjSpecialSpawnPos().y), b2Vec2(0.5, 0.5));
	for (Entity* player : players_) controllerTimes_.push_back(0);

	GameMode::initProgressBars();
}

void ControllerGameMode::update() {
	if (!roundFinished_) {
		if (controller_->isPicked()) {
			controllerTimes_[controller_->getPlayerId()] += (CONST(double, "MS_PER_FRAME"));
			if (controllerTimes_[controller_->getPlayerId()] >= CONST(double, "TIME_TO_WIN")) {
				roundFinished_ = true;
				winner_ = players_[controller_->getPlayerId()];
			}
		}
	}
}


void ControllerGameMode::render() {
	GameMode::renderProgressBars(controllerTimes_, CONST(double, "TIME_TO_WIN"));
	
	if (roundFinished_) {
		string winMsg = "Gana el jugador " + to_string(winner_->getComponent<PlayerData>(ComponentType::PlayerData)->getPlayerNumber()+1);
		Texture ganador(SDL_Game::instance()->getRenderer(), winMsg,
			SDL_Game::instance()->getFontMngr()->getFont(Resources::NES_Chimera), { COLOR(0xffffffff) });
		ganador.render(CONST(int, "WINDOW_WIDTH") / 2 - ganador.getWidth() / 2, CONST(int, "WINDOW_HEIGHT") / 2);
	}
}
