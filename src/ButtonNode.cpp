#include "ButtonNode.hpp"

HomieButtonNode::HomieButtonNode(const char *id, Type t)
: HomieNode(id, "button"), _type(t), _local(true), _on(false), _brightness(0), _target()
{
  this->subscribe("local", [this](String value) {
    bool v = value == F("true");
    if (!v && value != F("false"))
      return false;
    Serial.printf("Received order to turn %s local function\n", v ? "on" : "off");
    _local = v;
    if (v)
      this->triggerLocalTopic(_on, _brightness);
    Homie.setNodeProperty(*this, "local", _local ? "true" : "false");
    return true;
  });

  this->subscribe("target", [this](String value) {
    if (value.length() > 64)
      return false;
    Serial.printf("Button attached to node %s\n", value.c_str());
    _target = value;
    if (!_target.endsWith("/"))
      _target += "/";
    this->setOn(_on);
    if (this->is(eDimmer))
      this->setBrightness(_brightness);
    Homie.setNodeProperty(*this, "target", _target ? _target.c_str() : "");
    return true;
  });

  Homie.registerNode(*this);
}

void HomieButtonNode::setLocalTopicEnabled(bool v)
{
  _local = v;
}

void HomieButtonNode::setTargetTopic(String const &v)
{
  _target = v;
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
  Homie.publishProperty(_target, "on", v ? "true" : "false", true);
  _on = v;
  if (_local)
    triggerLocalTopic(_on, _brightness);
}

void HomieButtonNode::setBrightness(uint v)
{
  if (!is(eDimmer)) return;
  char szValue[8];
  sprintf(szValue, "%d", v);
  Homie.publishProperty(_target, "brightness", szValue, true);
  _brightness = v;
  if (_local)
    triggerLocalTopic(_on, _brightness);
}
