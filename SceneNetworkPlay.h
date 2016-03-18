#pragma once
#include "Scene.h"
#include "GameManager.h"
#include "Connection.h"
#include "Setting.h"

class SceneNetworkPlay : public Scene {

	GameManager m_gm;
	Connection  m_connection;
	Setting     m_setting;

	void processRecv();

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
