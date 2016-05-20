#ifndef HOMIE_BUTTON_NODE__INCLUDED__
#define HOMIE_BUTTON_NODE__INCLUDED__
#include <Arduino.h>
#include <Homie.h>
#include "RgbColor.hpp"

class HomieButtonNode: public HomieNode
{
public:
  enum Type {
    ePushButton,
    eSwitchButton,
    eDimmer,
  };

  HomieButtonNode(const char *id, Type t = ePushButton);

  bool is(Type t) const { return _type == t; }
  Type getType() const { return _type; }
  const char *getTypeName() const;

  // Returns true if the predefined local node associated with this button is enabled
  bool isLocalNodeEnabled() const { return _local; }
  void setLocalNodeEnabled(bool v);

  // Returns true if the button is pressed or switch on
  bool isOn() const { return _on; }

  // Returns the brightness of a dimmer
  int getBrightness() const { return _brightness; }

  // Gets the remote node this dimmer controls
  String getTargetTopic() const {}
  void setTargetTopic(String const &v);

protected:
  enum EUpdatedProperty
  {
    eLocalNodeEnabled,
    eOnChanged,
    eBrightnessChanged
  };
  
  void setOn(bool v);
  void setBrightness(uint brightness);
  virtual void updateLocalNode(EUpdatedProperty updatedProperty) {}
  virtual void onReadyToOperate();

private:
  const Type _type;
  bool _local;
  bool _on;
  int _brightness;
  String _target;
};
#endif
