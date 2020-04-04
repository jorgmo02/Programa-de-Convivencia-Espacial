#pragma once
#include <box2d.h>
#include <iostream>
#include "Health.h"
#include "AttachesToObjects.h"
#include "Weapon.h"
#include "Entity.h"
#include "Hands.h"
#include "Collider.h"
#include "PlayerData.h"
#include "GameMode.h"
#include "TileMap.h"

class RouterLogic;
class CollisionHandler :
	public b2ContactListener
{
private:
	struct weldData { //Struct donde guardamos los datos necesarios para hacer un weld.
		AttachesToObjects* player = nullptr;
		b2Body* bodyToBeAttached = nullptr;
		b2Vec2 collPoint;
	};
	vector<weldData> vecWeld; //Vector donde almacenamos los welds que realizaremos al final del step.
	struct moveData { //Struct donde guardamos los datos necesarios para respawnear a los jugadores
		b2Body* body;
		b2Vec2 pos;
	};
	vector<moveData> vecMove; //Vector donde almacenamos los moves que realizaremos al final del step.
	struct bodyData {  //Struct donde guardamos los datos necesarios para hacer un cuerpo muerto.
		b2Vec2 pos;
		float angle;
	};
	vector<bodyData> vecBody; //Vector donde almacenamos los cuerpos muertos que crearemos al final del step.
	vector<Weapon*> vecWeapon; //Vector donde almacenamos las weapons que soltaremos al final del step
	vector<AttachesToObjects*> vecAttach; //Vector donde almacenamos los agarres que soltaremos al final del step
	GameMode* gMode_;
	TileMap* tilemap_;

	void damageOnImpact(b2Fixture* fix, b2Fixture* player, Health* playerHealth);
public:

    CollisionHandler(GameMode* g, TileMap* tm): gMode_(g), tilemap_(tm) {};
	~CollisionHandler() {};

    void BeginContact(b2Contact* contact);

    void EndContact(b2Contact* contact);

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

	void SolveInteractions();

    bool ObjectCollidesWithPlayer(b2Fixture* fixA, Health*& player);

	bool PlayerCollidesWithRouterArea(b2Contact* contact, RouterLogic*& router, Collider*& collPlayer, PlayerData*& playerData);

	bool AttachableObjectCollidesWithPlayer(b2Fixture* fixA, AttachesToObjects*& player);
	
    bool PlayerCanPickWeapon(b2Contact* contact, Weapon* &pickableObj, Hands* &player);

	vector<bodyData> getBodyData() { return vecBody; }

	void clearBodyData(){ vecBody.clear(); }
};
