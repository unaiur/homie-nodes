#pragma once
#include "AddressableLedStrip.hpp"

namespace AddressableLedStrip
{
	class ShiftAnim: public Animation
	{
	public:

		ShiftAnim()
			: Animation(2000)
		{
		}
		
		void _update(Node &n, uint32_t progress)
		{
			unsigned numLeds = n.getLength();
			uint32_t onLeds = progress * numLeds / getDuration();
			if (n.isOn())
			{
				if (!n.isDirty()) n.clearTo(n.getCurrentColor(), 0, onLeds);
				n.clearTo(RgbColor(0), onLeds, numLeds);
			}
			else
			{
				n.clearTo(RgbColor(0), 0, onLeds);
			}
		}

		static void createFactory()
		{
			AnimationRegistry::power.addFactory<ShiftAnim>("shift");
		}
	};
}
