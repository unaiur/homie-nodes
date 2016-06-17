#pragma once
#include "LedStripNode.hpp"
#include "AddressableLedStrip/Animation.hpp"

namespace AddressableLedStrip
{
class Node: public HomieLedStripNode
{
public:
	enum LedStripTraits
	{
		eAnimable = eLastLedStripTrait,
		eLastAddressableLedStripTrait = eAnimable * 2
	};

	Node(const char *name, unsigned numLeds = 16, uint32_t traits = eDimmable | eSwitchable | eResizeable | eColorable | eAnimable);

	// Animation selection...
	bool setColorAnimation(const char *value);

	bool setUserAnimation(const char *value);

	bool setPowerAnimation(const char *value);
	
	inline RgbColor getCurrentColor() const;
	void setCurrentColor(RgbColor c);

	// Lighting functions
	virtual bool isDirty() const = 0;
	virtual RgbColor getPixel(uint16_t idx) const = 0;
	virtual void setPixel(uint16_t idx, RgbColor c) = 0;
	virtual void clearTo(RgbColor c, uint16_t from, uint16_t to) = 0;
	virtual void rotateLeft(uint16_t count, uint16_t from, uint16_t to) = 0;
	virtual void rotateRight(uint16_t count, uint16_t from, uint16_t to) = 0;
	virtual void shiftLeft(uint16_t count, uint16_t from, uint16_t to) = 0;
	virtual void shiftRight(uint16_t count, uint16_t from, uint16_t to) = 0;

	inline void clearTo(RgbColor c);
	inline void rotateLeft(uint16_t count);
	inline void rotateRight(uint16_t count);
	inline void shiftLeft(uint16_t count);
	inline void shiftRight(uint16_t count);
	
protected:
	virtual void onReadyToOperate();
	
	virtual void update(uint32_t source);

	virtual void loop();

	virtual void _enableLedBus() {}

	virtual void _disableLedBus() {}

	virtual bool canShow() const = 0;

	virtual void show() = 0;

private:
	void _publishAnimation(AnimationSelector const &anim);

	Animator _animator;

	RgbColor _currentColor; // Current color of the led strip while _colorAnimation is running
				// getColor() when _colorAnimation is over
};

RgbColor Node::getCurrentColor() const
{
	return _currentColor;
}

void Node::clearTo(RgbColor c)
{
	clearTo(c, 0, getLength());
}

void Node::rotateLeft(uint16_t count)
{
	rotateLeft(count, 0, getLength());
}

void Node::rotateRight(uint16_t count)
{
	rotateRight(count, 0, getLength());
}

void Node::shiftLeft(uint16_t count)
{
	shiftLeft(count, 0, getLength());
}

void Node::shiftRight(uint16_t count)
{
	shiftRight(count, 0, getLength());
}

}
