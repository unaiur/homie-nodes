#pragma once
#include <NeoPixelBus.h>
#include "AddressableLedStrip.hpp"

extern "C"
{
//    #include <eagle_soc.h>
//    #include <ets_sys.h>
    #include <uart.h>
    #include <uart_register.h>
}
namespace AddressableLedStrip
{
template <typename ColorType, typename MethodType>
class NeoPixelNode: public Node
{
	typedef NeoPixelBus<ColorType, MethodType> Bus;
public:
	NeoPixelNode(const char *name, unsigned numLeds = 16, uint traits = eDimmable | eSwitchable | eResizeable | eColorable | eAnimable)
		: Node(name, numLeds, traits)
		, _bus(new Bus(this->getLength(), 2))
		, _isBusEnabled(true)
		, _busStartMicros(micros())
		, _busNumFrames(0)
	{
		_bus->Begin();
	}

	bool isDirty() const
	{
		return _bus->IsDirty();
	}

	RgbColor getPixel(uint16_t idx) const
	{
		return _bus->GetPixelColor(idx);
	}

	void setPixel(uint16_t idx, RgbColor color)
	{
		_bus->SetPixelColor(idx, color);
	}

	void clearTo(RgbColor c, uint16_t from, uint16_t to)
	{
		_bus->ClearTo(c, from, to-1);
	}

	void rotateLeft(uint16_t count, uint16_t from, uint16_t to)
	{
		_bus->RotateLeft(count, from, to-1);
	}

	void rotateRight(uint16_t count, uint16_t from, uint16_t to)
	{
		_bus->RotateRight(count, from, to-1);
	}

	void shiftLeft(uint16_t count, uint16_t from, uint16_t to)
	{
		_bus->ShiftLeft(count, from, to-1);
	}

	void shiftRight(uint16_t count, uint16_t from, uint16_t to)
	{
		_bus->ShiftRight(count, from, to-1);
	}

protected:

	// Disable NeoPixelBus
	//
	// In all board based in ESP-12, there is led connected to TX1 which
	// turns on when the bus is idle.
	//
	// This code inverts TX1 output when idle to turn off that led.
	void _disableLedBus()
	{
		if (_isBusEnabled)
		{
			unsigned long elapsedUs = micros() - _busStartMicros;
			CLEAR_PERI_REG_MASK(UART_CONF0(UART1), (BIT(22)));
#ifdef SHOW_FRAME_RATE
			Serial.print(F("Last animation played "));
			Serial.print(_busNumFrames);
			Serial.print(F(" frames in "));
			Serial.print(elapsedUs);
			Serial.print(F(" at "));
			Serial.print(_busNumFrames * 1000000.0 / std::max<unsigned long>(1, elapsedUs));
			Serial.println(F(" frames per second."));
#endif
			_isBusEnabled = false;
		}
	}

	// Enable NeoPixelBus
	//
	// This code return NeoPixel bus to operational status
	void _enableLedBus()
	{
		if (!_isBusEnabled)
		{
			_isBusEnabled = true;
			_busNumFrames = 0;
			_busStartMicros = micros();
			SET_PERI_REG_MASK(UART_CONF0(UART1), (BIT(22)));
		}
	}

	bool canShow() const
	{
		// NeoPixels needs 50 microseconds to reset bus to the initial state after reenabling the bus
		return _isBusEnabled && micros() - _busStartMicros >= 50 && _bus->CanShow();
	}
	
	void show()
	{
		if (_bus->IsDirty())
		{
			_bus->Show();
			++_busNumFrames;
		}
	}

	void update(uint32_t src)
	{
		if (src == eResizeable)
		{
			delete _bus;
			_bus = new Bus(this->getLength(), 2);
			_bus->Begin();
			_bus->ClearTo(getCurrentColor());
			_isBusEnabled = true;
			_busNumFrames = 0;
			_busStartMicros = micros();
		}
		Node::update(src);
	}

private:
	Bus *_bus;
	bool _isBusEnabled;
	unsigned long _busStartMicros;
	unsigned _busNumFrames;
};
}
