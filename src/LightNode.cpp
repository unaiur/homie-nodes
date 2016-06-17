#include "LightNode.hpp"
#include "NeoPixelBus.h"

HomieLightNode::HomieLightNode(const char *id, uint32_t traits)
	: HomieNode(id, "light")
	, _traits(traits | eSwitchable)
	, _on(false)
	, _brightness(255)
	, _color(0xFFFFFF)
	, _effectiveColor(0)
{
	this->subscribe("on", [this](String value) {
		bool v = (value == F("true"));
		if (!v && value != F("false"))
			return false;
		//Serial.printf("Received order to turn %s light\n", v ? "on" : "off");
		setOn(v);
		return true;
	});

	if (is(eDimmable)) {
		this->subscribe("brightness", [this](String value) {
			char *err;
			auto v = strtoul(value.c_str(), &err, 10);
			if (*err || v > 255)
				return false;
			setBrightness(v);
			return true;
		});
	}

	if (is(eColorable)) {
		this->subscribe("color", [this](String value) {
			HtmlColor c;
			Serial.print(F("Received order to set color to "));
			Serial.println(value.c_str());
			if (!c.Parse(value.c_str()))
				return false;
			setColor(c);
			return true;
		});
	}
}

void HomieLightNode::_publishOn()
{
	Homie.setNodeProperty(*this, "on", this->_on ? "true" : "false");
}

void HomieLightNode::_publishBrightness()
{
	Homie.setNodeProperty(*this, "brightness", String(_brightness));
}

void HomieLightNode::_publishColor()
{
	char name[MAX_HTML_COLOR_NAME_LEN];
	_color.ToString(name, sizeof name);
	Homie.setNodeProperty(*this, "color", name);
}

void HomieLightNode::onReadyToOperate()
{
	this->_publishOn();
	if (is(eDimmable))
		this->_publishBrightness();
	if (is(eColorable))
		this->_publishColor();
}

void HomieLightNode::_updateEffectiveColor()
{
	if (_on)
	{
		int v = NeoGammaEquationMethod::Correct(_brightness);
		_effectiveColor = _color;
		_effectiveColor.R = (_effectiveColor.R * v + 255) / 256;
		_effectiveColor.G = (_effectiveColor.G * v + 255) / 256;
		_effectiveColor.B = (_effectiveColor.B * v + 255) / 256;
	}
	else
	{
		_effectiveColor = RgbColor(0);
	}
}

void HomieLightNode::setOn(bool v)
{
	if (_on != v)
	{
		_on = v;
		_updateEffectiveColor();
		this->update(eSwitchable);
		this->_publishOn();
	}
}

void HomieLightNode::setBrightness(int v)
{
	if (_brightness != v && v >= 0 && v <= 255)
	{
		_brightness = v;
		_updateEffectiveColor();
		this->update(eDimmable);
		this->_publishBrightness();
	}
}

void HomieLightNode::setColor(HtmlColor v)
{
	if (_color != v)
	{
		_color = v;
		_updateEffectiveColor();
		this->update(eColorable);
		this->_publishColor();
	}
}
