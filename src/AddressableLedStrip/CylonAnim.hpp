#pragma once
#include "AddressableLedStrip.hpp"

namespace AddressableLedStrip
{

	class CylonAnimation: public Animation
	{
		const unsigned ratio = 8;

	public:
		CylonAnimation()
			: Animation(2000, true)
		{
		}

		void start(Node &n)
		{
			Animation::start(n);
			_cylonUpwards = true;
			_cylonIdx = 0;
		}

		void _update(Node &n, uint32_t progress)
		{
			// Soften all the colors
			unsigned numLeds = n.getLength();
			unsigned step = progress * numLeds / getDuration();
			while (step > _cylonIdx)
			{
				for (unsigned i = 0; i < numLeds; ++i)
				{
					auto c = n.getPixel(i);
					if (c.R < ratio)
						c.R /= 2;
					else
						c.R -= c.R / ratio;
					if (c.G < ratio)
						c.G /= 2;
					else
						c.G -= c.G / ratio;
					if (c.B < ratio)
						c.B /= 2;
					else
						c.B -= c.B / ratio;
					n.setPixel(i, c);
				}

				// Bright the next one
				n.setPixel(_cylonUpwards ? _cylonIdx : numLeds - _cylonIdx - 1, n.getCurrentColor());
				++_cylonIdx;
			}

			if (progress == getDuration())
			{
				_cylonUpwards = !_cylonUpwards;
				_cylonIdx = 0;
			}
		}

		static void createFactory()
		{
			AnimationRegistry::user.addFactory<CylonAnimation>("cylon");
		}

	private:
		bool _cylonUpwards;
		unsigned _cylonIdx;
	};
}
