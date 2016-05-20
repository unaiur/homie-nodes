#include "ButtonNode.hpp"

HomieButtonNode::HomieButtonNode(const char *id, Type t)
: HomieNode(id, "button"), _type(t), _local(true), _on(false), _brightness(0), _target()
{
  this->subscribe("local", [this](String value) {
    bool v = value == F("true");
    if (!v && value != F("false"))
      return false;
    this->setLocalTopicEnabled(v);
    return true;
  });

  this->subscribe("target", [this](String value) {
    if (value.length() > 64)
      return false;
    this->setTargetTopic(value);
    return true;
  });

  Homie.registerNode(*this);
}

void HomieButtonNode::setLocalTopicEnabled(bool v)
{
  _local = v;
  Homie.setNodeProperty(*this, "local", _local ? "true" : "false");
  if (v)
    this->updateLocalTopic(eLocalTopicEnabled);
}

void HomieButtonNode::setTargetTopic(String const &v)
{
  Serial.printf("Button attached to node %s\n", v.c_str());
  _target = v;
  if (!_target.endsWith("/"))
    _target += "/";
  Homie.setNodeProperty(*this, "target", _target.length() > 0 ? _target.c_str() : "");
  Homie.publishProperty(_target, "on", isOn() ? "true" : "false", true);
  if (this->is(eDimmer))
  {
    char szValue[8];
    sprintf(szValue, "%d", getBrightness());
    Homie.publishProperty(_target, "brightness", szValue, true);
  }
}

const char *HomieButtonNode::getTypeName() const
{
  if (_type == ePushButton)
    return "push";
  if (_type == eSwitchButton)
    return "switch";
  if (_type == eDimmer)
    return "dimmer";
  return "unknown";
}

void HomieButtonNode::onReadyToOperate()
{
  Homie.setNodeProperty(*this, "local", _local ? "true" : "false");
  Homie.setNodeProperty(*this, "target", _target.length() > 0 ? _target.c_str() : "");
  Homie.setNodeProperty(*this, "type", getTypeName());
}

void HomieButtonNode::setOn(bool v)
{
  Serial.printf("Received order to turn %s local function\n", v ? "on" : "off");
  Homie.publishProperty(_target, "on", v ? "true" : "false", true);
  _on = v;
  if (_local)
    updateLocalTopic(eOnChanged);
}

void HomieButtonNode::setBrightness(uint v)
{
  if (!is(eDimmer)) return;
  char szValue[8];
  sprintf(szValue, "%d", v);
  Homie.publishProperty(_target, "brightness", szValue, true);
  _brightness = v;
  if (_local)
    updateLocalTopic(eBrightnessChanged);
}
