#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneNetworkPlay.h"
#include "SceneLocalSelect.h"
#include <cstdio>
using namespace std;


SceneManager::SceneManager(){
	m_scene_id      = NONE;
	m_next_scene_id = TITLE;
}
void SceneManager::setNextScene( int scene_id ){
	m_next_scene_id = scene_id;
}

int  SceneManager::update(){
	// �V�[���J�ڔ���
	if( m_scene_id != m_next_scene_id ){
		// �I������
		fin();
		m_scene_id = m_next_scene_id;
		// ����������
		switch( m_scene_id ){
		case TITLE:
			SceneTitleInit(); break;
		case NETWORK_PLAY:
			SceneNetworkPlayInit(); break;
		case LOCAL_SELECT:
			SceneLocalSelectInit(); break;
		default:
			puts("error:����`�̃V�[��");
		}
	}
	// �X�V����
	switch( m_scene_id ){
	case TITLE:
		return SceneTitleUpdate();
	case NETWORK_PLAY:
		return SceneNetworkPlayUpdate();
	case LOCAL_SELECT:
		return SceneLocalSelectUpdate();
	default:
		puts("error:����`�̃V�[��");
		return -1;
	}
	return 0;
}

void SceneManager::draw(){
	switch( m_scene_id ){
	case TITLE:
		SceneTitleDraw(); break;
	case NETWORK_PLAY:
		SceneNetworkPlayDraw(); break;
	case LOCAL_SELECT:
		SceneLocalSelectDraw(); break;
	default:
		puts("error:����`�̃V�[��");
	}
}

void SceneManager::fin(){
	switch( m_scene_id ){
	case TITLE:
		SceneTitleFin(); break;
	case NETWORK_PLAY:
		SceneNetworkPlayFin(); break;
	}

}

