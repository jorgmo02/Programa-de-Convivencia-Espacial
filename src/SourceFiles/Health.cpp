#include "Health.h"
#include <iostream>
#include "Collider.h"
#include "Collision.h"
#include "Weapon.h"
#include "MeleeWeapon.h"
#include "ExtinguisherWeapon.h"
#include "AttachesToObjects.h"
#include "CollisionHandler.h"
#include "ThrownByPlayer.h"
#include "PlayerController.h"
#include"AnimatedPlayer.h"

Health::Health(int l) : Component(ComponentType::Health)
{
	lives_ = livesMax_ = l;
}

void Health::init() {
	//col_ = GETCMP1_(Collider);
	//col_->setUserData(this);
	INV_FRAMES_HIT_ = CONST(int, "INVULNERABILITY_FRAMES_HIT");
	INV_FRAMES_RESPAWN_ = CONST(int, "INVULNERABILITY_FRAMES_RESPAWN");
	invFrames_ = 0;
	animSpeed_ = CONST(int, "INVULNERABILITY_ANIM_SPEED");

	loopsInv_ = (INV_FRAMES_RESPAWN_ / CONST(int, "NFRAMES_ANIM3")) / 10; //el numero de loops que tiene que hacer la animacion
	anim_ = entity_->getComponent<AnimatedPlayer>(ComponentType::AdvancedAnimatedViewer);
}

void Health::update()
{
	if (invFrames_ > 0) {
		invFrames_--;
	}
}

Health::~Health()
{
}

bool Health::subtractLife(int damage)
{
	if (invFrames_ <= 0 && damage > 0) {
		if (lives_ > 0) {
			lives_ -= damage;
			invFrames_ = INV_FRAMES_HIT_;
			//darle a la animacion
			anim_->startAnimation(loopsInv_, 0, -1, 3);
			anim_->setAnimSpeed(animSpeed_);

			SDL_Game::instance()->getAudioMngr()->playChannel(Resources::DeathSound, 0);

			if (lives_ <= 0) {
				lives_ = 0;
				SDL_Game::instance()->getAudioMngr()->playChannel(Resources::RespawnSound, 0);
				return false;
			}	//Evitar vidas negativas
			return true;
		}
		else return false;
	}
	else return true;
}

void Health::addLife(int sum)
{
	if (lives_ + sum > livesMax_) lives_ = livesMax_;
	else lives_ += sum;
}

void Health::playerDead(CollisionHandler* c)
{
	//reset player

	//soltar objetos agarrados
	AttachesToObjects* a = GETCMP1_(AttachesToObjects);
	if (a != nullptr && a->isAttached())
		c->breakAttachment(a);
	//soltar arma
	Hands* h = GETCMP1_(Hands);
	Weapon* w = nullptr;
	if (h != nullptr) w = h->getWeapon();
	if (w != nullptr) c->dropWeapon(w);
	//reset impulso
	PlayerController* pc = GETCMP1_(PlayerController);
	pc->resetImpulseForce();

	//cuerpo muerto
	CollisionHandler::bodyData body;
	b2Fixture* fix = GETCMP1_(Collider)->getFixture(0);
	body.pos = fix->GetBody()->GetPosition();
	body.angle = GETCMP1_(Collider)->getAngle();
	body.linearVelocity = fix->GetBody()->GetLinearVelocity();
	body.angularVelocity = fix->GetBody()->GetAngularVelocity();
	cout << " player dead with angle: " << body.angle << endl;
	c->addCorpse(body);

	//respawn
	GameMode* s = c->getGamemode();
	PlayerData* p = GETCMP1_(PlayerData);
	CollisionHandler::moveData mov;
	mov.body = GETCMP1_(Collider)->getFixture(0)->GetBody();
	mov.pos = c->getPlayerRespawnPoint(p->getPlayerNumber());
	if (s->onPlayerDead(p->getPlayerNumber())) {	//avisa al jugador si puede respawnear
		resetHealth();
		invFrames_ = INV_FRAMES_RESPAWN_;
	}
	else {	//si no le quedan vidas le mandamos lejos provisionalmente
		mov.pos = b2Vec2((1 + p->getPlayerNumber()) * 50, 0);
	}
	c->addMove(mov);
}

void Health::onCollisionEnter(Collision* c)
{
	if (invFrames_ <= 0) {
		b2Fixture* fix = c->hitFixture;
		if (!fix->IsSensor())
		{
			b2Vec2 force = c->hitFixture->GetBody()->GetLinearVelocity();
			int impact;

			Weapon* w = GETCMP_FROM_FIXTURE_(fix, Weapon);
			PlayerData* playerWhoHitMe = nullptr;

			//¿Es un objeto lanzable? (Por lo tanto puede desarmar)
			ThrownByPlayer* thrown = GETCMP_FROM_FIXTURE_(fix, ThrownByPlayer);

			//Cogemos nuestras manos
			Hands* h = GETCMP1_(Hands);
			impact = force.Length();
			//Comprobamos si el golpe con el arma es suficientemente fuerte (indica que se ha lanzado o llevaba impulso)
			if (impact >= CONST(double, "DISARM_IMPACT") && thrown != nullptr && thrown->getOwnerId() != h->getPlayerId()) {
				//Nos desarman con el golpe
				Weapon* we = nullptr;
				if (h != nullptr) we = h->getWeapon();

				//Si tenemos un arma cogida la soltamos al haber sido golpeados
				if (we != nullptr) c->collisionHandler->dropWeapon(we);
			}

			//Lo que ha golpeado es un arma?
			if (w != nullptr) {
				force *= w->getImpactForce();
				impact = force.Length();
				//Si se impacta con un arma al umbral m�s alto de fuerza, se recibe su daño de impacto
				impact = (impact >= CONST(double, "HIGH_DAMAGE")) ? w->getImpactDamage() : 0;
			}
			else {
				force *= c->hitFixture->GetBody()->GetMass();
				impact = force.Length();
				//Depending on the force of impact we apply damage to the player
				if (impact < CONST(int, "LOW_DAMAGE")) impact = 0;

				else if (impact >= CONST(int, "LOW_DAMAGE") && impact < CONST(double, "MEDIUM_DAMAGE")) impact = 1;

				else if (impact >= CONST(double, "MEDIUM_DAMAGE") && impact < CONST(double, "HIGH_DAMAGE")) impact = 2;

				else if (impact >= CONST(double, "HIGH_DAMAGE")) impact = 3;

				// we get the Id of the player who hit me at high speed
				// (it may not exist, so we check later that it's not nullptr
				playerWhoHitMe = GETCMP_FROM_FIXTURE_(fix, PlayerData);
			}

			if (!subtractLife(impact)) {
				ThrownByPlayer* objThrown = GETCMP_FROM_FIXTURE_(fix, ThrownByPlayer);
				// player is killed by a weapon
				if (objThrown != nullptr) {
					if (objThrown->getOwnerId() != GETCMP1_(PlayerData)->getPlayerNumber())
						objThrown->addPointsToOwner();
				}

				// player is killed by another player at high speed
				else if (playerWhoHitMe != nullptr) {
					if (playerWhoHitMe->getPlayerNumber() != GETCMP1_(PlayerData)->getPlayerNumber()) {
						GameMode* s = c->collisionHandler->getGamemode();
						s->playerKillsPlayer(playerWhoHitMe->getPlayerNumber());
					}
				}

				playerDead(c->collisionHandler);
			}
		}
	}
}