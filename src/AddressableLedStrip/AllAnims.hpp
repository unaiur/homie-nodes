#include "LinearColorFadeAnim.hpp" 
#include "ShiftAnim.hpp" 
#include "CylonAnim.hpp" 
#include "FireAnim.hpp" 

namespace AddressableLedStrip
{
	inline void createAllAnimFactories()
	{
		LinearColorFadeAnim::createFactory();
		ShiftAnim::createFactory();
		CylonAnimation::createFactory();
		FireAnimation::createFactory();
	}
};
