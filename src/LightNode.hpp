#ifndef HOMIE_LIGHT_NODE__INCLUDED__
#define HOMIE_LIGHT_NODE__INCLUDED__
#include <Arduino.h>
#include <Homie.h>
#include "RgbColor.hpp"

class HomieLightNode: public HomieNode
{
public:
  enum Trait {
    eSwitchable = 1, // mandatory trait
    eDimmable = 2,
    eColorable = 4,
  };

  HomieLightNode(const char *id, uint traits = 0);

  bool is(Trait t) const { return (_traits & t) != 0; }

  bool isOn() const { return _on; }
  void setOn(bool v);

  int getBrightness() const { return _brightness; }
  void setBrightness(int v);

  RgbColor getColor() const { return _color; }
  void setColor(RgbColor v);

protected:
  virtual void onReadyToOperate();
  virtual void update(Trait source) = 0;

private:
  const uint _traits;
  bool _on;
  int _brightness;
  RgbColor _color;
};
#endif
