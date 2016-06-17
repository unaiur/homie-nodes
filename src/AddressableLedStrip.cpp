#include "AddressableLedStrip.hpp"

namespace AddressableLedStrip
{

Node::Node(const char *name, unsigned numLeds, uint32_t traits)
	: HomieLedStripNode(name, numLeds, traits)
	, _animator()
	, _currentColor(0)
{
	if (this->is(eAnimable)) {
		this->subscribe(_animator._colorAnimation.getName(), [this](String value) {
			this->setColorAnimation(value.c_str());
			return true;
		});
		this->subscribe(_animator._powerAnimation.getName(), [this](String value) {
			this->setPowerAnimation(value.c_str());
			return true;
		});
		Serial.print("Subscribing to ");
		Serial.println(_animator._userAnimation.getName());
		this->subscribe(_animator._userAnimation.getName(), [this](String value) {
			this->setUserAnimation(value.c_str());
			return true;
		});
	}
	Serial.flush();
}

void Node::onReadyToOperate()
{
	HomieLedStripNode::onReadyToOperate();
	this->_publishAnimation(_animator._colorAnimation);
	this->_publishAnimation(_animator._userAnimation);
	this->_publishAnimation(_animator._powerAnimation);
}

void Node::update(uint32_t source)
{
	if (source & (eColorable|eDimmable))
	{
		_enableLedBus();
		if (!_animator.startColorAnimation(*this))
			setCurrentColor(getEffectiveColor());
	}
	if (source & eSwitchable)
	{
		_enableLedBus();
		_animator.startPowerAnimation(*this);
		if (isOn())
			_animator.startUserAnimation(*this);
		else
			_animator.stopUserAnimation(*this);
		if (!_animator.isActive())
			setCurrentColor(getEffectiveColor());
	}
}

void Node::loop()
{
	if (canShow())
	{
		if (_animator.isActive())
		{
			show();
			_animator.loop(*this);
		}
		else if (isDirty())
		{
			show();
		}
		else
		{
			_disableLedBus();
		}
	}
}


void Node::setCurrentColor(RgbColor c)
{
	_currentColor = c;
	if (!_animator.isUserAnimationActive())
		clearTo(c); 
}

void Node::_publishAnimation(AnimationSelector const &selector)
{
	Homie.setNodeProperty(*this, selector.getName(), selector.getFactory());
}

bool Node::setColorAnimation(const char *value)
{
	if (!_animator.setColorAnimation(value))
		return false;
	_publishAnimation(_animator._colorAnimation);
	return true;
}

bool Node::setUserAnimation(const char *value)
{
	if (!_animator.setUserAnimation(value))
		return false;
	if (isOn() && _animator.startUserAnimation(*this))
		_enableLedBus();
	_publishAnimation(_animator._userAnimation);
	return true;
}

bool Node::setPowerAnimation(const char *value)
{
	if (!_animator.setPowerAnimation(value))
		return false;
	_publishAnimation(_animator._powerAnimation);
	return true;
}

}
