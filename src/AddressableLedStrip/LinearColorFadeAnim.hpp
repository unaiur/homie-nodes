#pragma once
#include "AddressableLedStrip.hpp"

namespace AddressableLedStrip
{
	class LinearColorFadeAnim: public Animation
	{
	public:
		LinearColorFadeAnim()
			: Animation(2000)
		{
		}
		
		void start(Node &n)
		{
			Animation::start(n);
			_originColor = n.getCurrentColor();
		}

		void _update(Node &n, uint32_t progress)
		{
			n.setCurrentColor(RgbColor::LinearBlend(_originColor, n.getEffectiveColor(), progress / float(getDuration())));
		}

		static void createFactory()
		{
			AnimationRegistry::color.addFactory<LinearColorFadeAnim>("linear");
		}

		RgbColor _originColor;
	};
}
