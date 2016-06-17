#pragma once
#include <Arduino.h>

namespace AddressableLedStrip
{
class AnimationFactory;
class AnimationRegistry;
class Node;

class Animation
{
public:
	inline Animation(uint32_t duration, bool restart = false);

	virtual void start(Node &n);

	virtual void stop(Node &n);

	inline bool isActive() const;

	inline uint32_t getDuration() const;

	inline void update(Node &n);

protected:
	virtual void _update(Node &n, uint32_t progress) = 0;

	uint32_t _updateProgress();


private:
	bool _active;
	bool _restart;
	uint32_t _msStartTime;
	uint32_t _msDuration;
	
};

class AnimationFactory
{
	friend class AnimationRegistry;
public:
	inline AnimationFactory(const char *name);

	inline const char *getName() const;

	virtual Animation *create() const = 0;

private:
	const char * const _name;
	AnimationFactory *_next;
};

template <typename T>
struct TAnimationFactory: public AnimationFactory
{
	inline TAnimationFactory(const char *name);
	inline Animation *create() const;
};

class AnimationRegistry
{
public:
	inline AnimationRegistry(const char *name);

	inline const char *getName() const;

	AnimationFactory const *findFactory(const char *name) const;

	Animation *createRandomAnimation() const;

	void addFactory(AnimationFactory *factory);

	template <typename Anim>
	void addFactory(const char *name)
	{
		addFactory(new TAnimationFactory<Anim>(name));
	}

	static AnimationRegistry color;
	static AnimationRegistry user;
	static AnimationRegistry power;

private:
	const char *_name;
	AnimationFactory *_first, *_last;
	uint32_t _numFactories;
};

class AnimationSelector
{
public:
	inline AnimationSelector(AnimationRegistry const &registry);

	inline const char *getName() const;

	bool setFactory(const char *name);

	const char *getFactory() const;

	bool start(Node &n);

	inline void update(Node &n);

	inline void stop(Node &n);

	inline bool isActive() const;

private:
	bool _randomize;
	bool _recreate;
	Animation *_animation;
	AnimationFactory const *_factory;
	AnimationRegistry const &_registry;
};

class Animator
{
	friend class Node;
public:
	Animator();
	
	inline bool setColorAnimation(const char *name);
	inline bool startColorAnimation(Node &n);
	inline bool isColorAnimationActive() const;

	inline bool setUserAnimation(const char *name);
	inline bool startUserAnimation(Node &n);
	inline void stopUserAnimation(Node &n);
	inline bool isUserAnimationActive() const;

	inline bool setPowerAnimation(const char *name);
	inline bool startPowerAnimation(Node &n);
	inline bool isPowerAnimationActive() const;

	inline bool isActive() const;
	void loop(Node &n);
	inline void stop(Node &n);

private:
	bool _startAnimation(uint16_t index, AnimationSelector &selector);

	AnimationSelector _colorAnimation;
	AnimationSelector _userAnimation;
	AnimationSelector _powerAnimation;
};

Animation::Animation(uint32_t duration, bool restart)
	: _msDuration(duration)
	, _restart(restart)
	, _active()
{
}

bool Animation::isActive() const
{
	return _active;
}

uint32_t Animation::getDuration() const
{
	return _msDuration;
}

void Animation::update(Node &n)
{
	if (isActive())
		_update(n, _updateProgress());
}

AnimationFactory::AnimationFactory(const char *name)
	: _name(name), _next()
{
}

const char *AnimationFactory::getName() const
{
	return _name;
}

template <typename T>
TAnimationFactory<T>::TAnimationFactory(const char *name)
	: AnimationFactory(name)
{
}

template <typename T>
Animation *TAnimationFactory<T>::create() const
{
	return new T();
}

AnimationRegistry::AnimationRegistry(const char *name)
	: _name(name), _first(), _last(), _numFactories()
{
}

const char *AnimationRegistry::getName() const
{
	return _name;
}

AnimationSelector::AnimationSelector(AnimationRegistry const &registry)
	: _randomize(), _recreate(), _animation(), _factory(), _registry(registry)
{
}

const char *AnimationSelector::getName() const
{
	return _registry.getName();
}

void AnimationSelector::update(Node &n)
{
	if (_animation)
		_animation->update(n);
}

void AnimationSelector::stop(Node &n)
{
	if (_animation)
		_animation->stop(n);
}

bool AnimationSelector::isActive() const
{
	return _animation && _animation->isActive();
}

bool Animator::setColorAnimation(const char *name)
{
	return _colorAnimation.setFactory(name);
}
bool Animator::startColorAnimation(Node &n)
{
	return _colorAnimation.start(n);
}
bool Animator::isColorAnimationActive() const
{
	return _colorAnimation.isActive();
}

bool Animator::setUserAnimation(const char *name)
{
	return _userAnimation.setFactory(name);
}
bool Animator::startUserAnimation(Node &n)
{
	return _userAnimation.start(n);
}
void Animator::stopUserAnimation(Node &n)
{
	_userAnimation.stop(n);
}
bool Animator::isUserAnimationActive() const
{
	return _userAnimation.isActive();
}

bool Animator::setPowerAnimation(const char *name)
{
	return _powerAnimation.setFactory(name);
}
bool Animator::startPowerAnimation(Node &n)
{
	return _powerAnimation.start(n);
}
bool Animator::isPowerAnimationActive() const
{
	return _powerAnimation.isActive();
}

bool Animator::isActive() const
{
	return isColorAnimationActive() || isUserAnimationActive() || isPowerAnimationActive();
}

void Animator::stop(Node &n)
{
	_colorAnimation.stop(n);
	_userAnimation.stop(n);
	_powerAnimation.stop(n);
}
}
