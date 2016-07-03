#include <Arduino.h>
#include <Homie.h>
#include <Bounce2.h>
#include <LedStripNode.hpp>
#include <ButtonNode.hpp>
#include "NeoPixelLedStrip.hpp"
#include <AddressableLedStrip/AllAnims.hpp>
#include <Wire.h>
#include <APDS9930.h>

const int APDS_INT = 14;

AddressableLedStrip::NeoPixelNode<NeoGrbFeature, NeoEsp8266AsyncUart800KbpsMethod> *myLight;

class MyButton: public HomieButtonNode
{
public:
	MyButton(HomieLightNode *n)
		: HomieButtonNode("button", n, eDimmer)
	{
		pinMode(APDS_INT, INPUT);
		attachInterrupt(digitalPinToInterrupt(APDS_INT), _interruptHandler, FALLING);
	}

protected:
	void setup()
	{
		_lastRead = millis();
		Serial.print(F("APDS-9930: "));
		if (!_sensor.init())
		{
			Serial.println(F("[NOT DETECTED]"));
		}
		else
		{
			if (_sensor.enableProximitySensor(true))
				Serial.println(F(" [DONE]"));
			else
				Serial.println(F(" [FAIL]"));
			_sensor.wireWriteDataByte(APDS9930_PTIME, 0xFC);
			_sensor.wireWriteDataByte(APDS9930_POFFSET, 0x20);
			_state = eIdle;
			_lastToggle = millis() - 1000;
		}
	}

	bool readProximitySensor()
	{
		const unsigned scale = 4 * 128; // Value 0-7
		uint16_t value;
		if (!_sensor.readProximity(value))
			return false;

		value /= scale;

		if (value == 0)
		{
			_sensor.setProximityIntLowThreshold(0);
			_sensor.setProximityIntHighThreshold(scale + scale * 3 / 4);
		}
		else
		{
			_sensor.setProximityIntLowThreshold(value * scale - scale * 3 / 4);
			_sensor.setProximityIntHighThreshold((value + 1) * scale + scale * 3 / 4);
		}
			
		_sensor.clearProximityInt();
		if (value == _lastValue)
			return false;

		_lastValue = value;
		_isr_flag = false;
		_lastRead = millis();

		if (_state == eIdle && _lastValue > 0)
		{
			_wakingTime = millis();
			_maxValue = _lastValue;
			_state = eWakingUp;
		}
		else if (_lastValue > _maxValue)
		{
			_maxValue = _lastValue;
		}
			
		Serial.printf("New proximity value %u [max: %u, state: %u]\n", _lastValue, _maxValue, _state);
		Homie.setNodeProperty(*this, "prox", String(_lastValue));
			
		return true;
	}

	void increaseBrightness(int v)
	{
		int brightness = v + getBrightness();
		brightness = std::max(0, std::min(255, brightness));
		Serial.printf("Adjust brightness %d => %u\n", v, brightness);
		this->setBrightness(brightness);
	}

	void loop()
	{
		if (_isr_flag)
			readProximitySensor();

		if (_state == eWakingUp)
		{
			if (_lastValue == 0)
			{
				if (millis() - _lastToggle > 1000)
				{
					Serial.println("Toggle light");
					this->setOn(!isOn());
					_state = eIdle;
					_lastToggle = millis();
				}
			}
			else if (millis() - _wakingTime > 1000)
			{
				Serial.println("Adjusting brightness");
				_state = eActive;
				if (!isOn())
					setOn(true);
			}
		}

		if (_state == eActive)
		{
			auto ms = millis() - _lastRead;
			if (_lastValue == 0 && ms >= 1800)
			{
				Serial.println("Brightness adjusted");
				_state = eIdle;
			}
			else if ((_lastValue == 1 || _lastValue == 2 || _lastValue == 7) && ms >= 10)
			{
				increaseBrightness(_lastValue < 4 ? -1 : 1);
				_lastRead += 10;
			}
			else if ((_lastValue == 3 || _lastValue == 6) && ms >= 50)
			{
				increaseBrightness(_lastValue < 4 ? -1 : 1);
				_lastRead += 50;
			}
			else if ((_lastValue == 4 || _lastValue == 5) && ms >= 100)
			{
				increaseBrightness(_lastValue < 4 ? -1 : 1);
				_lastRead += 100;
			}
		}
		
	}
	
private:
	static void ICACHE_RAM_ATTR _interruptHandler()
	{
  		_isr_flag = true;
	}

	enum State
	{
		eIdle,
		eWakingUp,
		eActive,
	};
	APDS9930 _sensor;
	uint32_t _lastRead, _wakingTime, _lastToggle;
	uint16_t _lastValue, _maxValue;
	uint8_t _state;
	static bool _isr_flag;
};
MyButton *myButton;
bool MyButton::_isr_flag;

void setup()
{
	Serial.begin(115200);
	AddressableLedStrip::createAllAnimFactories();
	myLight = new AddressableLedStrip::NeoPixelNode<NeoGrbFeature, NeoEsp8266AsyncUart800KbpsMethod>("leds");
	myButton = new MyButton(myLight);
	Serial.flush();

	Homie.setFirmware("ledstrip", "1.0.0");
	Homie.disableResetTrigger();
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
