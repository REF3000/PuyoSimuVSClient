#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneNetworkPlay.h"
#include "SceneLocalSelect.h"
#include <cstdio>
using namespace std;


Scene *newScene( eSCENE scene_id ){
	switch( scene_id ){
	case TITLE       : return new SceneTitle();
	case NETWORK_PLAY: return new SceneNetworkPlay();
	case LOCAL_SELECT: return new SceneLocalSelect();
	default:
		puts("error:未定義のシーン");
		return new SceneTitle();
	}
}

SceneManager::SceneManager(){
	m_scene = newScene( TITLE );
	m_scene->init( NULL );
}

void SceneManager::setNextScene( eSCENE scene_id ){
	m_next_scene_id = scene_id;
}

void SceneManager::setParam( PARAM param ){
	m_param = param;
}

int  SceneManager::update(){
	// シーン遷移判定
	if( m_next_scene_id != NONE ){
		// 終了処理
		m_scene->fin();
		delete m_scene;
		// 初期化処理
		m_scene = newScene( m_next_scene_id );
		m_scene->init( m_param );
		m_next_scene_id = NONE;
	}
	// 更新処理
	return m_scene->update();
}

void SceneManager::draw(){
	m_scene->draw();
}

void SceneManager::fin(){
	m_scene->fin();
}

