#ifndef HOMIE_LIGHT_NODE__INCLUDED__
#define HOMIE_LIGHT_NODE__INCLUDED__
#include <Arduino.h>
#include <Homie.h>
#include <internal/HtmlColor.h>

class HomieLightNode: public HomieNode
{
public:
	enum Trait {
		eSwitchable = 1, // mandatory trait
		eDimmable = 2,
		eColorable = 4,
		eLastLightTrait = 8
	};

	HomieLightNode(const char *id, uint32_t traits = 0);

	bool is(Trait t) const { return (_traits & t) != 0; }
	bool is(int t) const { return (_traits & t) != 0; }

	bool isOn() const { return _on; }
	void setOn(bool v);

	int getBrightness() const { return _brightness; }
	void setBrightness(int v);

	HtmlColor getColor() const { return _color; }
	void setColor(HtmlColor v);

	RgbColor getEffectiveColor() const { return _effectiveColor; }
	uint32_t getTraits() const { return _traits; }

protected:
	virtual void onReadyToOperate();
	virtual void update(uint32_t source) = 0;
	void _publishOn();
	void _publishBrightness();
	void _publishColor();

private:
	void _updateEffectiveColor();
	const uint32_t _traits;
	bool _on;
	uint8_t _brightness;
	HtmlColor _color;
	RgbColor _effectiveColor;
};
#endif
