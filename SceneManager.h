#include "Scene.h"

enum eSCENE{
	NONE, TITLE, NETWORK_PLAY, LOCAL_SELECT
};

class SceneManager{

	eSCENE m_next_scene_id;
	Scene *m_scene;
	PARAM m_param;

public:
	SceneManager();
	void setNextScene( eSCENE scene_id );
	void setParam( PARAM param );

	int  update(); // 0à»äOÇ≈èIóπ
	void draw();
	void fin();

};
