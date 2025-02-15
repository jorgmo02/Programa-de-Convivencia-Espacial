#pragma once
#include "Component.h"
#include "Viewer.h"
#include "AbstractAnimatedViewers.h"

class AdvancedAnimatedViewer : public Viewer, public AbstractAnimatedViewers
{
protected:
	int anim_ = 0; //animación actual
	vector<AnimationInfo> animationsInfo_;

public:
	AdvancedAnimatedViewer(int textureId, int timePerFrame);

	~AdvancedAnimatedViewer();

	// make sure to load frames as in AnimatedPlayer.cpp
	virtual void init() override = 0;
	virtual void update() override;

	int getCurrentAnim() const { return anim_; }

	virtual void startAnimation(int loops = -1, int initialFrame = 0, int limitFrame = -1, int animation = 0) override {
		loops_ = loops;
		currentLoop_ = 0;
		frame_ = initialFrame;
		limitFrame_ = limitFrame;
		anim_ = animation;
		activeAnimation_ = true;
	}

	virtual void resetAnimation();
};