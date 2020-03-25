#include "CollisionHandler.h"
//This method calculates the damage recieved by the impact of an object (or another player) with the player

void CollisionHandler::damageOnImpact(b2Fixture* fix, Health* playerHealth) {
	//Measure de impact of an object with the player 
	b2Vec2 force = fix->GetBody()->GetMass() * fix->GetBody()->GetLinearVelocity();

	int impact = force.Length();

	//Depending on the force of impact we apply damage to the player
	if (impact >= CONST(int, "LOW_DAMAGE")&& CONST(double, "MEDIUM_DAMAGE")) {playerHealth->subtractLife(1); }

	if (impact >= CONST(double, "MEDIUM_DAMAGE") && impact < CONST(double, "HIGH_DAMAGE")) {playerHealth->subtractLife(2);}

	if (impact >= CONST(double, "HIGH_DAMAGE")) {playerHealth->subtractLife(3);}
}

//Handles start of collisions
void CollisionHandler::BeginContact(b2Contact* contact)
{
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();
	Health* player_Health = nullptr;
	AttachesToObjects* player_AttachesToObjects = nullptr;
	Weapon* pickableObj = nullptr;
	Hands* playerHands = nullptr;

	//Comprueba que FixA es el jugador, que FixB es un trigger, que el jugador está presionando la tecla A (mando) o Space (teclado) y que no está agarrado a nada más.
	if (AttachableObjectCollidesWithPlayer(fixA, player_AttachesToObjects) && fixB->GetFilterData().categoryBits == Collider::CollisionLayer::NormalAttachableObject && player_AttachesToObjects->canAttachToObject()) { 
		b2WorldManifold manifold; //Una manifold es un registro donde se guardan todas las colisiones
		contact->GetWorldManifold(&manifold); //Obtenemos la manifold global
		weldData newWeld; //Struct donde guardamos los datos necesarios para crear un weld
		newWeld.player = player_AttachesToObjects;
		newWeld.bodyToBeAttached = fixB->GetBody();
		newWeld.collPoint = b2Vec2(manifold.points[0].x, manifold.points[0].y); //Punto de colisi�n. En cualquier colisi�n siempre hay 2 puntos de colisi�n. Con 1 nos basta.
		vecWeld.push_back(newWeld); //Metemos el weldData en el vector. La raz�n por la que no hacemos el joint ya es porque no se puede crear un joint en medio de un step.
	}
	else if (fixB->GetFilterData().categoryBits){
		//check collision then do whatever, in this case twice because it might be two players colliding 
		if (ObjectCollidesWithPlayer(fixA, player_Health)) {
			damageOnImpact(fixB, player_Health);	//Check the stats of the other object
		}

		player_Health = nullptr;	//Lo reseteamos para evitar problemas

		if (ObjectCollidesWithPlayer(fixB, player_Health)) {
			damageOnImpact(fixA, player_Health);	//Check the stats of the other object
		}
	}

	//Pickable weapon collisions
	if ((contact->GetFixtureA()->GetFilterData().categoryBits == Collider::CollisionLayer::Weapon ||
		contact->GetFixtureB()->GetFilterData().categoryBits == Collider::CollisionLayer::Weapon) &&
		PlayerCanPickWeapon(contact, pickableObj, playerHands)) {
		pickableObj->SavePlayerInfo(playerHands->getPlayerId(), playerHands);
	}

	
}

//Handles end of collisions
void CollisionHandler::EndContact(b2Contact * contact){
	//Pickable weapons
	Weapon* pickableObj = nullptr;
	Hands* playerHands = nullptr;
	if ((contact->GetFixtureA()->GetFilterData().categoryBits == Collider::CollisionLayer::Weapon ||
		contact->GetFixtureB()->GetFilterData().categoryBits == Collider::CollisionLayer::Weapon) &&
		PlayerCanPickWeapon(contact, pickableObj, playerHands)) {
		pickableObj->DeletePlayerInfo(playerHands->getPlayerId());
		cout << "Dropped weapon" << endl;
	}
}

//If you want to disable a collision after it's detected
void CollisionHandler::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}

//Gather info about impulses
void CollisionHandler::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}

//to add a new collision behaviour, make a method that checks if it's the specific collision you want
//you can distinguish bodies by their user data or make them collide with certain objects only with collision layers
//if you need to use a component you have to do collider->setUserData(this) in the component's init first
bool CollisionHandler::ObjectCollidesWithPlayer(b2Fixture* fixA, Health*& player)
{	
	//Obtenemos los datos guardados en el Collider
	Entity* aux = static_cast<Entity*>(fixA->GetBody()->GetUserData());

	if(aux != nullptr){		//Cuidado de que no sea null

	//Cogemos el health si es que lo tiene
	player = aux->getComponent<Health>(ComponentType::Health);
	}

	if (player != nullptr) {	//Si lo tiene es que es un player
		return true;
	}
	else return false;
}

bool CollisionHandler::PlayerCanPickWeapon(b2Contact* contact, Weapon* &pickableObj, Hands* &player) {
	Entity* fixAentity = static_cast<Entity*>(contact->GetFixtureA()->GetBody()->GetUserData());
	Entity* fixBentity = static_cast<Entity*>(contact->GetFixtureB()->GetBody()->GetUserData());

	if ((fixAentity->hasComponent(ComponentType::Weapon)) &&
		(pickableObj = static_cast<Weapon*>(fixAentity->getComponent<Weapon>(ComponentType::Weapon))) &&
		fixBentity->hasComponent(ComponentType::Hands)) {

		player = static_cast<Hands*>(fixBentity->getComponent<Hands>(ComponentType::Hands));
		return true;
	}
	else if ((fixBentity->hasComponent(ComponentType::Weapon)) &&
		(pickableObj = static_cast<Weapon*>(fixBentity->getComponent<Weapon>(ComponentType::Weapon))) &&
		fixAentity->hasComponent(ComponentType::Hands)){

		player = static_cast<Hands*>(fixAentity->getComponent<Hands>(ComponentType::Hands));
		return true;
	}
	return false;
}

bool CollisionHandler::AttachableObjectCollidesWithPlayer(b2Fixture* fixA, AttachesToObjects*& player) {
	return (player = static_cast<AttachesToObjects*>(static_cast<Entity*>(fixA->GetBody()->GetUserData())->getComponent<AttachesToObjects>(ComponentType::AttachesToObjects)));
}

void CollisionHandler::SolveInteractions() {
	for (int k = 0; k < vecWeld.size(); k++) { //Recorre el vector resolviendo todos los joint y lo limpia al final.
		vecWeld[k].player->attachToObject(vecWeld[k].bodyToBeAttached, vecWeld[k].collPoint);
	}
	vecWeld.clear();
}
