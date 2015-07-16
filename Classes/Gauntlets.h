#include "Weapon.h"
#include "cocos2d.h"

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