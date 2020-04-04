#pragma once
#include <list>
#include "GameState.h"
#include "GameMode.h"
#include "Entity.h"
#include "checkML.h"
#include "collisionHandler.h"

using namespace std;

// esta clase es equivalente a un nivel del juego
//se espera que herede de gamestate en un futuro
//tambien deberia convertirse en un template de modo de juego y mapa

class PlayState : public GameState
{
private:
	b2World* physicsWorld_;
	vector<b2Body*> physicalEntities_; //almacena los punteros a los colliders de b2
	//puede que no sea necesario si cogemos la referencia en cuanto los creamos con addPhysicalEntity
	b2Body* addPhysicalEntity(/*PAR�METROS PARA PODER CREAR UN BODY*/) {};  //a�ade una entidad fisica
	//HUD

	CollisionHandler* collisionHandler_;
	vector<Entity*> deadBodies;
	vector<Collider*> collDeadBodies;
	vector<Entity*> players_;
	GameMode* gameMode_ = nullptr;
	string tilemapName_;
	Texture* fondo_;
	std::map<string, int> resourceMap_ = { //añadir aquí los mapas que se vayan haciendo
		{"SalaDeEstar", Resources::SalaDeEstar}
	};
	TileMap* tilemap_;

public:
	PlayState(GameMode* gMode, string tmap);
	~PlayState();
	virtual void init();
	virtual void update();
	void createDeadBodies();
	virtual void render();
	virtual void handleInput();
	std::vector<Entity*> getPlayers() { return players_; };
	EntityManager* getEntityManager() { return entityManager_; }
	b2World* getPhysicsWorld() { return physicsWorld_; }
};
