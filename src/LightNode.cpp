#include "LightNode.hpp"

HomieLightNode::HomieLightNode(const char *id, uint traits)
  : HomieNode(id, "light"), _traits(traits | eSwitchable), _on(false), _brightness(100), _color(255,255,255)
{
  this->subscribe("on", [this](String value) {
    bool v = value == F("true");
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
      if (*err || v > 100)
        return false;
      setBrightness(v);
      return true;
    });
  }

  if (is(eColorable)) {
    this->subscribe("color", [this](String value) {
      RgbColor c;
      Serial.printf("Received order to set color to %s\n", value.c_str());
      if (!c.parse(value))
        return false;
      setColor(c);
      return true;
    });
  }

  Homie.registerNode(*this);
}

void HomieLightNode::onReadyToOperate()
{
  String traitList;
  if (is(eDimmable)) {
    traitList = F("dimmable");
  }
  if (is(eColorable)) {
    if (traitList) traitList += ',';
    traitList += F("rgb");
  }
  Homie.setNodeProperty(*this, "traits", traitList, true);
}

void HomieLightNode::setOn(bool v)
{
  if (_on == v) return;
  _on = v;
  this->update(eSwitchable);
  Homie.setNodeProperty(*this, "on", v ? "true" : "false");
}

void HomieLightNode::setBrightness(int v)
{
  if (_brightness == v || v < 0 || v > 100) return;

  _brightness = v;
  this->update(eDimmable);
  Homie.setNodeProperty(*this, "brightness", String(v));
}

void HomieLightNode::setColor(RgbColor v)
{
  if (_color == v) return;
  _color = v;
  this->update(eColorable);
  Homie.setNodeProperty(*this, "color", v.toString());
}
