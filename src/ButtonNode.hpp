#ifndef HOMIE_BUTTON_NODE__INCLUDED__
#define HOMIE_BUTTON_NODE__INCLUDED__
#include <Arduino.h>
#include <Homie.h>
#include <LightNode.hpp>

/* Base class to implement a button node
 *
 * There are three types of buttons:
 *   - Push buttons, which turns off automatically as soon the user removes the finger
 *   - Switches, which retains the state
 *   - Potentiometers, which can adjust an intensity or brigthness value from 0% (fully off) to 100% (fully on)
 */
class HomieButtonNode: public HomieNode
{
public:
  enum Type {
    ePushButton,
    eSwitchButton,
    eDimmer,
  };

  HomieButtonNode(const char *id, HomieLightNode *attachedNode = 0, Type t = ePushButton);

  bool is(Type t) const { return _type == t; }
  Type getType() const { return _type; }
  const char *getTypeName() const;

  // Returns true if the predefined local node associated with this button is enabled
  bool isLocalNodeEnabled() const { return _local; }
  void setLocalNodeEnabled(bool v);

  // Returns true if the button is pressed or switch on
  bool isOn() const { return _local ? _attachedNode->isOn() :  _on; }

  // Returns the brightness of a dimmer
  int getBrightness() const { return _local ? _attachedNode->getBrightness() : _brightness; }

  // Gets the remote node this dimmer controls
  HomieLightNode *getAttachedNode() const { return _attachedNode; }
  String const &getAttachedNodePath() const;

protected:
  void setOn(bool v);
  void setBrightness(uint brightness);
  void setAttachedNodePath(String const &path);
  String getNodeTopic(const char *nodeId);
  virtual void setup();
  virtual void onReadyToOperate();

private:
  const Type _type;
  bool _local;
  bool _on;
  int _brightness;
  HomieLightNode *_attachedNode;
  String _attachedNodePath;
};
#endif
