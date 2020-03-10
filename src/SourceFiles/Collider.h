#pragma once
#include "Component.h"
#include "box2d.h"
#include "SDL_rect.h"
#include "Constants.h"


class Collider : public Component
{
private:
	//general
	b2Body* body_;
	b2World* world_;
	b2BodyDef bodyDef_;

	//fixtures
	vector<float> widths_;
	vector<float> heights_;
	vector<b2PolygonShape> shapes_;
	vector<b2FixtureDef> fixtureDefs_;
	vector<b2Fixture*> fixtures_;

public:
	//Add the different collision layers as we see fit
	enum CollisionLayer {
		Normal = 0x0001, //a collision layer can't be zero or else it won't collide
		Player,
		Trigger
	};

	//Friccion -> rozamiento al contacto con otros cuerpos   Drag-> rozamiento con el aire
	Collider(b2World* world, b2BodyType type, float x, float y, float width, float height,
		float density, float friction, float restitution, float linearDrag, float angDrag, CollisionLayer c, bool sensor);

	~Collider() {
		world_->DestroyBody(body_); world_ = nullptr;
	}

	//getters
	float getW(int i) const { return widths_[i]; }
	float getH(int i) const { return heights_[i]; }
	b2Vec2 getPos() const { return body_->GetPosition(); }
	b2Vec2 getLinearVelocity() const { return body_->GetLinearVelocity(); }
	b2BodyType getType() const { return body_->GetType(); }
	b2Transform getTransform() const { return body_->GetTransform(); }
	float getAngle() const { return body_->GetAngle(); }
	float getMass() const { return body_->GetMass(); }
	bool isAwake() const { return body_->IsAwake(); }
	bool isSensor(int i) const { return fixtureDefs_[i].isSensor; }
	bool isEnabled() const { return body_->IsEnabled(); }
	bool isBullet() const { return body_->IsBullet(); }
	SDL_Rect getRect() const {
		return SDL_Rect{
			(int)getPos().x, (int)getPos().y,
			(int)getW(0), (int)getH(0)
		};
	}

	SDL_Rect getRectRender() const {
		return SDL_Rect{
			(int)(getPos().x * PIXELS_PER_METER - (getW(0) * PIXELS_PER_METER)),
			(int)(WINDOW_HEIGHT - (getPos().y * PIXELS_PER_METER + (getH(0) * PIXELS_PER_METER))),
			(int)(getW(0) * PIXELS_PER_METER * 2),
			(int)(getH(0) * PIXELS_PER_METER * 2)
		};
	}

	//setters
	void setType(b2BodyType t) { body_->SetType(t); }
	void setAwake(bool b) { body_->SetAwake(b); }
	void setEnabled(bool b) { body_->SetEnabled(b); }
	void setBullet(bool b) { body_->SetBullet(b); }
	void setUserData(void* data) { body_->SetUserData(data); }
	void setTransform(b2Vec2 pos, float angle) { body_->SetTransform(pos, angle); }
	void setLinearVelocity(b2Vec2 vel) { body_->SetLinearVelocity(vel); }
	void applyLinearImpulse(b2Vec2 force, b2Vec2 point) { body_->ApplyLinearImpulse(force, body_->GetPosition() + point, true); }
	void applyForce(b2Vec2 force, b2Vec2 point) { body_->ApplyForce(force, body_->GetPosition() + point, true); }
	void createFixture(float width, float height, float density,
		float friction, float restitution, CollisionLayer, bool sensor);
	void destroyFixture(int i);
};
