#include "ButtonNode.hpp"

HomieButtonNode::HomieButtonNode(const char *id, HomieNode *attachedNode, Type t)
: HomieNode(id, "button"), _type(t), _local(true), _on(false), _brightness(0)
, _attachedNode(attachedNode), _attachedNodePath()
{
  if (_attachedNode) {
    this->subscribe("local", [this](String value) {
      bool v = value == F("true");
      if (!v && value != F("false"))
        return false;
      this->setLocalNodeEnabled(v);
      return true;
    });
  }

  this->subscribe("remote", [this](String value) {
    if (value.length() > 64)
      return false;
    this->setAttachedNodePath(value);
    return true;
  });
}

void HomieButtonNode::setup()
{
  if (_attachedNode)
    _attachedNodePath = getNodeTopic(_attachedNode->getId());
}
void HomieButtonNode::setLocalNodeEnabled(bool v)
{
  _local = v;
  Homie.setNodeProperty(*this, "local", _local ? "true" : "false");
  if (v)
    this->updateLocalNode(eLocalNodeEnabled);
}

String HomieButtonNode::getNodeTopic(const char *nodeId)
{
  String topic(64);
  topic = Homie.getBaseTopic();
  topic.concat(Homie.getId());
  topic.concat('/');
  topic.concat(nodeId);
  topic.concat('/');
  return topic;
}

void HomieButtonNode::setAttachedNodePath(String const &v)
{
  Serial.print(F("Button attached to node "));
  Serial.println(v.c_str());
  if (v.length() == 0 && _attachedNode)
    _attachedNodePath = getNodeTopic(_attachedNode->getId());
  else
    _attachedNodePath = v;
  Homie.setNodeProperty(*this, "remote", _attachedNodePath.length() > 0 ? _attachedNodePath.c_str() : "");
  if (_attachedNodePath)
    Homie.publishRaw((_attachedNodePath + F("on/set")).c_str(), isOn() ? "true" : "false", true);
  if (this->is(eDimmer))
  {
    char szValue[8];
    sprintf(szValue, "%d", getBrightness());
    Homie.publishRaw((_attachedNodePath + F("brightness/set")).c_str(), szValue, true);
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
  Homie.setNodeProperty(*this, "remote", _attachedNodePath.length() > 0 ? _attachedNodePath.c_str() : "");
  Homie.setNodeProperty(*this, "type", getTypeName());
}

void HomieButtonNode::setOn(bool v)
{
  Serial.printf("Received order to turn %s local function\n", v ? "on" : "off");
  _on = v;
  if (_local)
    updateLocalNode(eOnChanged);
  if (_attachedNodePath)
    Homie.publishRaw((_attachedNodePath + F("on/set")).c_str(), v ? "true" : "false", true);
}

void HomieButtonNode::setBrightness(uint v)
{
  if (!is(eDimmer)) return;
  _brightness = v;
  if (_local)
    updateLocalNode(eBrightnessChanged);
  if (_attachedNodePath) {
    char szValue[8];
    sprintf(szValue, "%d", v);
    Homie.publishRaw((_attachedNodePath + F("brightness/set")).c_str(), szValue, true);
  }
}
