#include <Arduino.h>
#include <Homie.h>
#include <Bounce2.h>
#include "LightNode.hpp"
#include "ButtonNode.hpp"

const int PIN_RELAY = 2;
const int PIN_BUTTON = 0;
class MyLight: public HomieLightNode
{
public:
  MyLight(): HomieLightNode("light", eDimmable|eColorable)
  {
  }

  void setup()
  {
    pinMode(PIN_RELAY, OUTPUT);
    analogWriteRange(100);
    analogWrite(PIN_RELAY, 100);
  }

protected:
  void update(Trait)
  {
    int v = isOn() ? getBrightness() : 0;
    Serial.printf("Setting light brightness to %d\n", v);
    analogWrite(PIN_RELAY, 100 -v);
  }
};
MyLight myLight;

class MyButton: public HomieButtonNode
{
public:
  MyButton(): HomieButtonNode("button", eSwitchButton)
  {
  }

  void setup()
  {
    _debouncer.attach(PIN_BUTTON, INPUT_PULLUP);
    _debouncer.interval(100);
  }

  void loop()
  {
    _debouncer.update();
    if (_debouncer.fell()) {
      bool state = this->isLocalTopicEnabled() ? myLight.isOn() : this->isOn();
      this->setOn(!state);
    }
  }

  void updateLocalTopic(EUpdatedProperty updatedProperty)
  {
    if (updatedProperty == eOnChanged)
    {
      myLight.setOn(isOn());
    }
    else if (updatedProperty == eBrightnessChanged)
    {
      myLight.setBrightness(getBrightness());
    }
    else if (updatedProperty == eLocalTopicEnabled)
    {
      this->setOn(myLight.isOn());
      this->setBrightness(myLight.getBrightness());
    }
  }
protected:
  Bounce _debouncer;
};
MyButton myButton;

void setup()
{
  Serial.begin(115200);
  Homie.setFirmware("ledstrip", "1.0.0");
  Homie.setup();
}

void loop()
{
  Homie.loop();
#ifdef ENABLE_ARDUINO_OTA
  void handleOTA();
  handleOTA();
#endif
}
