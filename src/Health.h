#pragma once
#include "Component.h"
#include "checkML.h"


class Health: public Component
{
public:
	Health(int l);
	~Health();

	void resetHealth() { lives_ = livesMax_; }
	bool subtractLife(int damage); //return true si despues del da�o sigue vivo
	void addLife(int sum); //suma sum a la vida siempre y cuando esta no supere la constanteGeneral
	int getHealthMax() { return livesMax_; }
private:
	int lives_;
	int livesMax_;
};

