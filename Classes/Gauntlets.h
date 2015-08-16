#include "Weapon.h"
#include "cocos2d.h"

/**
 * 
 * ½ğÊôÊÖÌ×ÎäÆ÷
 * 
*/	
class Gauntlets : public Weapon
{
public:
	Gauntlets();
	~Gauntlets();

	bool init();

	void reset();

	void cleanup();

	CREATE_FUNC(Gauntlets);

	
protected:
private:
};