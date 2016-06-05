#ifndef HOMIE_LED_STRIP_NODE__INCLUDED__
#define HOMIE_LED_STRIP_NODE__INCLUDED__
#include "LightNode.hpp"

class HomieLedStripNode: public HomieLightNode
{
public:
  enum LedStripTraits
  {
	eResizeable = eLastLightTrait,
	eLastLedStripTrait = eResizeable * 2
  };

  HomieLedStripNode(const char *id, int length, uint traits = eDimmable | eSwitchable | eResizeable);

  int getLength() const { return _length; }
  virtual void setLength(int v);

protected:
  virtual void onReadyToOperate();

  void _publishLength();

  int _length;
};

#endif // HOMIE_LED_STRIP_NODE__INCLUDED__
