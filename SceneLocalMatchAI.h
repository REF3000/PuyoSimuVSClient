#include "Scene.h"
#include "GameManager.h"
#include "PuyoAI.h"

class SceneLocalMatchAI : public Scene {

	GameManager m_gm;
	PuyoAI *m_ai;

	Action getAiAction();

public:

	void init( PARAM param );
	int update();
	void draw();
	void fin();

};
