#include "Scene.h"

class SceneLocalSelect : public Scene {

	int m_select;

public:

	void init( PARAM param );
	int update();
	void draw();
	void fin();

};
