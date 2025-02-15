#include "Weapon.h"
#include <iostream>
#include "InputBinder.h"
#include "Hands.h"
#include "Health.h"
#include "Wallet.h"
#include "Collision.h"
#include "ThrownByPlayer.h"

int Weapon::calculateCoinsDropped(int coinsPlayer)
{
	if (coinsPlayer <= CONST(int, "WEAPON_HIGH_TIER_COIN_DAMAGE")) return 3;
	else if (coinsPlayer <= CONST(int, "WEAPON_MID_TIER_COIN_DAMAGE")) return 2;
	else if (coinsPlayer > CONST(int, "WEAPON_MID_TIER_COIN_DAMAGE")) return 1;
	return 0;
}

void Weapon::init()
{
	maxThrowSpeed_ = CONST(double, "WEAPON_MAX_THROW_SPEED");
	minThrowSpeed_ = CONST(double, "WEAPON_MIN_THROW_SPEED");
	spinOnThrowSpeed_ = CONST(double, "WEAPON_SPIN_SPEED");

	mainCollider_ = GETCMP1_(Collider);
	vw_ = GETCMP1_(Viewer);
	mainCollider_->createCircularFixture(2, 0, 0, 0, Collider::CollisionLayer::Trigger, true);
	//Tamaño del vector segun el numero de jugadores
	playerInfo_.resize(4);

	entity_->getEntityManager()->getWeaponVector()->push_back(this);

	throwCooldown_ = CONST(int, "THROW_COOLDOWN");

	framesUntilRecoveringCollision_ = CONST(int, "RECOVERING_COLLISION_AFTER_THROWN_COOLDOWN");
}

void Weapon::handleInput()
{
	if (currentHand_ == nullptr) {
		for (int i = 0; i < playerInfo_.size(); i++) {
			if (!IsPicked() && playerInfo_[i].isNear &&
				playerInfo_[i].playerHands->getCanPickWeapon() && playerInfo_[i].playerBinder->pressPick()) {
				PickObjectBy(i);
			}
		}
	}
	else if (throwCooldownTimer_ >= throwCooldown_ && IsPicked() && playerInfo_[pickedIndex_].playerBinder->pressThrow())
	{
		throwCooldownTimer_ = 0;
		UnPickObject();
	}
}

void Weapon::PickObjectBy(int index)
{
	Hands* playerH = playerInfo_[index].playerHands;
	if (playerH->getWeaponID() == NoWeapon) {
		currentHand_ = playerH;
		picked_ = true;
		pickedIndex_ = index;
		currentHand_->setWeapon(weaponType_, this);
		mainCollider_->getBody()->SetEnabled(false);
		vw_->setDrawable(false);

		hasBeenThrownRecently_ = false;
		framesUntilRecoveringCollisionTimer_ = 0;
		mainCollider_->changeLayerCollision(0, Collider::CollisionLayer::Player);

		ThrownByPlayer* throwData = GETCMP1_(ThrownByPlayer);
		throwData->SetOwner(index);

		SDL_Game::instance()->getAudioMngr()->playChannel(Resources::PickSound, 0);
	}
}

void Weapon::separateWeapon(double resultThrowSpeed) {
	//Si se tira un objeto, se guarda en el objeto lanzado la ID de quien lo lanza.
	GETCMP1_(ThrownByPlayer)->throwObject(pickedIndex_);

	// desactiva colisión con el jugador
	b2Filter f = mainCollider_->getFilterFromLayer(Collider::CollisionLayer::NormalObject);
	uint16 playerLayer = Collider::CollisionLayer::Player1 * pow(2, getPlayerId());
	f.maskBits -= playerLayer;
	mainCollider_->getFixture(0)->SetFilterData(f);
	hasBeenThrownRecently_ = true;
	//

	// pone la mano sin ningún arma
	currentHand_->setWeapon(NoWeapon, nullptr);
	currentHand_->setCanPickWeapon(false);
	picked_ = false;
	pickedIndex_ = -1;
	mainCollider_->getBody()->SetEnabled(true);
	vw_->setDrawable(true);
	mainCollider_->setLinearVelocity(b2Vec2(0, 0));

	// mueve el arma y la impulsa en función de resultThrowSpeed
	mainCollider_->setTransform(b2Vec2(currentHand_->getPos().x + currentHand_->getDir().x * currentHand_->getArmLengthPhysics(), currentHand_->getPos().y - currentHand_->getDir().y * currentHand_->getArmLengthPhysics()), currentHand_->getAngle());
	mainCollider_->applyLinearImpulse(b2Vec2(currentHand_->getDir().x * resultThrowSpeed, -currentHand_->getDir().y * resultThrowSpeed), mainCollider_->getBody()->GetLocalCenter());

	// reinicia arma
	currentHand_->setFrame(0, 0);
	currentHand_->stopAnimation();
	currentHand_ = nullptr;
}

void Weapon::UnPickObject()
{
	double actualMagnitude = currentHand_->getVel().Length();
	double resultThrowSpeed = minThrowSpeed_ +
		((actualMagnitude * (maxThrowSpeed_ - minThrowSpeed_)) / 10/*Media de magnitud maxima del jugador*/);
	/*Hay que tener en cuenta el tamaño de la fixture principal del arma*/
	float tam = mainCollider_->getW(0) + mainCollider_->getH(0);
	resultThrowSpeed *= tam;

	separateWeapon(resultThrowSpeed);
	mainCollider_->getBody()->SetAngularVelocity(spinOnThrowSpeed_);
}

void Weapon::letFallObject() {
	if (picked_) separateWeapon(1.5);
}

int Weapon::getPlayerId() {
	return currentHand_->getPlayerId();
}

void Weapon::onCollisionEnter(Collision* c)
{
	Entity* other = c->entity;
	Hands* otherHand = GETCMP2(other, Hands);
	Collider* coll = GETCMP1_(Collider);
	b2Fixture* auxF = c->myFixture;

	if (otherHand != nullptr &&
		auxF->GetFilterData().categoryBits == Collider::CollisionLayer::Trigger) {
		SavePlayerInfo(otherHand->getPlayerId(), otherHand, GETCMP2(other, Health), GETCMP2(other, Wallet), GETCMP2(other, PlayerData)->getBinder());
	}
}

void Weapon::onCollisionExit(Collision* c)
{
	Hands* otherHand = GETCMP2(c->entity, Hands);
	b2Fixture* auxF = c->myFixture;

	if (otherHand != nullptr &&
		auxF->GetFilterData().categoryBits == Collider::CollisionLayer::Trigger) {
		DeletePlayerInfo(otherHand->getPlayerId());
	}
}

void Weapon::SavePlayerInfo(int index, Hands* playerH, Health* healthAux, Wallet* walletAux, InputBinder* binderAux)
{
	playerInfo_[index].isNear = true;
	playerInfo_[index].playerHands = playerH;
	playerInfo_[index].playerBinder = binderAux;
	if (healthAux) playerInfo_[index].playerHealth = healthAux;
	else playerInfo_[index].playerWallet = walletAux;
}

void Weapon::DeletePlayerInfo(int index)
{
	playerInfo_[index].isNear = false;
	playerInfo_[index].playerHands = nullptr;
}

void Weapon::update() {
	if (picked_)
		throwCooldownTimer_++;

	// recupera colisiones con todos los jugadores tras cierto tiempo
	if (hasBeenThrownRecently_) {
		framesUntilRecoveringCollisionTimer_++;
		if (framesUntilRecoveringCollisionTimer_ >= framesUntilRecoveringCollision_) {
			hasBeenThrownRecently_ = false;
			framesUntilRecoveringCollisionTimer_ = 0;
			if (currentHand_ == nullptr) {
				mainCollider_->getFixture(0)->SetFilterData(mainCollider_->getFilterFromLayer(Collider::CollisionLayer::NormalObject));
				mainCollider_->getFixture(1)->SetFilterData(mainCollider_->getFilterFromLayer(Collider::CollisionLayer::Trigger));
			}
		}
	}
}