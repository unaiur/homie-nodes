#include <AddressableLedStrip/Animation.hpp>

namespace AddressableLedStrip
{

AnimationRegistry AnimationRegistry::color("colorAnimation");
AnimationRegistry AnimationRegistry::power("powerAnimation");
AnimationRegistry AnimationRegistry::user("userAnimation");

void Animation::start(Node &)
{
	_active = true;
	_msStartTime = millis();
}

void Animation::stop(Node &)
{
	_active = false;
}

uint32_t Animation::_updateProgress()
{
	auto diff = millis() - _msStartTime;
	if (diff < _msDuration)
		return diff;
	if (_restart)
		_msStartTime += _msDuration;
	else
		_active = false;
	return _msDuration;
}

AnimationFactory const *AnimationRegistry::findFactory(const char *name) const
{
	for (auto f = _first; f != NULL; f = f->_next)
		if (strcasecmp(f->getName(), name) == 0)
			return f;
	return NULL;
}

Animation *AnimationRegistry::createRandomAnimation() const
{
	auto f = _first;
	for (uint32_t i = random(_numFactories); i; --i)
		f = f->_next;
	return f ? f->create() : NULL;
}

void AnimationRegistry::addFactory(AnimationFactory *factory)
{
	if (_last)
		_last->_next = factory;
	else
		_first = factory;
	_last = factory;
	++_numFactories;
}

bool AnimationSelector::setFactory(const char *name)
{
	if (strcasecmp(name, "random") == 0)
	{
		_randomize = true;
		return true;
	}
	if (*name == 0 || strcasecmp(name, "none") == 0)
	{
		_factory = 0;
		_randomize = false;
		return true;
	}
	auto f = _registry.findFactory(name);
	if (f)
	{
		_recreate = (f != _factory);
		_factory = f;
		_randomize = false;
		return true;
	}
	return false;
}

const char *AnimationSelector::getFactory() const
{
	if (_randomize)
		return "random";
	if (!_factory)
		return "none";
	return _factory->getName();
}

bool AnimationSelector::start(Node &n)
{
	if (_randomize)
	{
		// Random animation. Delete old one and create a random one
		if (_animation)
		{
			_animation->stop(n);
			delete _animation;
		}
		_animation = _registry.createRandomAnimation();
	}
	else if (!_factory)
	{
		// Animation was disabled. Delete it
		if (_animation)
		{
			_animation->stop(n);
			delete _animation;
			_animation = NULL;
		}
	}
	else if (_recreate)
	{
		// Animation has changed. Delete it and replace by a new one.
		if (_animation)
		{
			_animation->stop(n);
			delete _animation;
		}
		_animation = _factory->create();
		_recreate = false;
	}
	if (_animation)
		_animation->start(n);
	return _animation;
}

Animator::Animator()
	: _colorAnimation(AnimationRegistry::color)
	, _userAnimation(AnimationRegistry::user)
	, _powerAnimation(AnimationRegistry::power)
{
}

void Animator::loop(Node &n)
{
	_colorAnimation.update(n);
	_userAnimation.update(n);
	_powerAnimation.update(n);
}

}
