#include "Scene.h"
#include "GameManager.h"

class SceneLocalMatchAI : public Scene {

	GameManager m_gm;

public:

	void init( PARAM param );
	int update();
	void draw();
	void fin();

};
