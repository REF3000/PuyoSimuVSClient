#pragma once
#include "Scene.h"

enum eSCENE{
	NONE, TITLE, NETWORK_PLAY, LOCAL_SELECT, LOCAL_MATCH_AI, LOCAL_MATCH_TOKOPUYO
};

class SceneManager{

	eSCENE m_next_scene_id;
	Scene *m_scene;
	PARAM m_param;

public:
	SceneManager();
	void setNextScene( eSCENE scene_id );
	void setParam( PARAM param );

	int  update(); // 0�ȊO�ŏI��
	void draw();
	void fin();

};
