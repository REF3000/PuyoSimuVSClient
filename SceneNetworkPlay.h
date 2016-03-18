#pragma once
#include "Scene.h"
#include "GameManager.h"
#include "Connection.h"
#include "Setting.h"

class SceneNetworkPlay : public Scene {

	GameManager m_gm;
	Connection  m_connection;
	Setting     m_setting;

	bool        m_action_flag;
	Action      m_enemy_action;
	bool        m_begin_turn_first;

	void processRecv();
	void matchingNotice();
	void setEnemyName( char buf[], int len );
	void setNextTable( char buf[] );
	void beginTurn();
	void setEnemyAction( char buf[] );
public:

	void init( PARAM param );
	int update();
	void draw();
	void fin();

};

class SceneNetworkPlay2 : public Scene {

public:

	void init( PARAM param );
	int update();
	void draw();
	void fin();

};
