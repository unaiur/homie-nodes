#include "LedStripNode.hpp"

HomieLedStripNode::HomieLedStripNode(const char *id, int length, uint traits)
  : HomieLightNode(id, traits), _length(length)
{
  this->subscribe("length", [this](String value) {
      char *err;
      auto v = strtoul(value.c_str(), &err, 10);
      if (*err || v > 1000)
        return false;
      setLength(v);
      return true;
  });
}

void HomieLedStripNode::_publishLength()
{
  Homie.setNodeProperty(*this, "brightness", String(_length));
}

void HomieLedStripNode::setLength(int v)
{
  if (_length == v || v < 0 || v > 1000) return;

  _length = v;
  this->update(eResizeable);
  this->_publishLength();
}

void HomieLedStripNode::onReadyToOperate()
{
  HomieLightNode::onReadyToOperate();
  this->_publishLength();
}
