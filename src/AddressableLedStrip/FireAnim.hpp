#pragma once
#include "AddressableLedStrip.hpp"

namespace AddressableLedStrip
{
	class FireAnimation: public Animation
	{
		static uint16_t coldDown(uint16_t x)
		{
			const unsigned ratio = 64;
			return (x < ratio ? !!x : x / ratio);
		}
		static RgbColor temp2color(uint16_t temp, uint8_t brightness)
		{
			uint8_t r = std::min<uint32_t>(255, temp);
			temp -= r;
			uint8_t g = std::min<uint32_t>(255, temp);
			temp -= g;
			r = (r * uint32_t(brightness) + 255) / 256;
			g = (g * uint32_t(brightness) + 255) / 512;
			temp = (temp * uint32_t(brightness) + 255) / 1024;
			return RgbColor(r, g, temp);
		}

	public:
		FireAnimation()
			: Animation(2048, true)
		{
		}

		void start(Node &n)
		{
			Animation::start(n);
			_temp = new uint16_t[n.getLength()];
			memset(_temp, 0, sizeof(uint16_t) * n.getLength());
			_lastSpark = _lastFrame = millis();
		}

		void stop(Node &n)
		{
			n.clearTo(n.getCurrentColor());
			Animation::stop(n);
			delete [] _temp;
		}

		void _update(Node &n, uint32_t progress)
		{
			uint32_t now = millis();
			if (now - _lastFrame < 10)
				return;
			_lastFrame = now;
			unsigned numLeds = n.getLength();
		
			// Spread heat to neighbours
			for (unsigned i = 1; i < numLeds; ++i)
			{
				int diff = int(_temp[i-1]) - int(_temp[i]);
				if (diff > 1)
				{
					diff = std::max<unsigned>(2, diff / 8);
					_temp[i]    += diff-1;
					_temp[i-1]  -= diff-1;
				}
				else if (diff == 1)
				{
					//_temp[i-1]  -= 1;
					_temp[i]  -= 0;
				}
				else if (diff == 0)
				{
					if (_temp[i] > 2)
						_temp[i] -= 2;
					else if (_temp[i] == 2)
						_temp[i] -= 1;
				}
				else if (diff == -1)
				{
					//_temp[i-1]  -= 0;
					_temp[i]  -= 1;
				}
				else
				{
					diff = std::max<unsigned>(2, -diff / 8);
					_temp[i]    -= diff-1;
					_temp[i-1]  += diff-1;
				}
			}

			_temp[0] -= coldDown(_temp[0]);
#if 0
			// Cold down...
			for (unsigned i = 0; i < numLeds; ++i)
				_temp[i] -= coldDown(_temp[i]);
#endif

			// Create new sparks
			RgbColor c = n.getColor();
			if (n.isOn() && now - _lastSpark > random(100*c.R))
			{
				_lastSpark = now;
				uint32_t pos = random(numLeds);
				_temp[pos] = std::min<uint16_t>(3*255, std::max<uint16_t>(3*255, _temp[pos]) + random(255));
			}
			
			// Convert temperatures to colors
			uint8_t brightness = n.getBrightness();
			for (unsigned i = 0; i < numLeds; ++i)
				n.setPixel(i, temp2color(_temp[i], brightness));
		}

		static void createFactory()
		{
			AnimationRegistry::user.addFactory<FireAnimation>("fire");
		}

	private:
		uint16_t *_temp;
		uint32_t _lastSpark;
		uint32_t _lastFrame;
	};

}
