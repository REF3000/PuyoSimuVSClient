#include "SceneLocalSelect.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "input.h"
#include <iostream>
using namespace std;

extern SceneManager scene_manager;

const char NAME_LIST[][64] = { "Player1 vs Player1�i�Ƃ��Ղ�j", "Player1 vs AI_01", "Player1 vs AI_02�i�������j" };
const int  NAME_LIST_SIZE = 3;

void SceneLocalSelect::init( PARAM param ){   // ������ �N�����ɌĂ�
	m_select = 0;
}

int  SceneLocalSelect::update(){ // �X�V ���t���[���Ă�
	if( GetStateKey(KEY_INPUT_ESCAPE) == 1 ){
		scene_manager.setNextScene( TITLE );
		return 0;
	}
	if( GetStateKey(KEY_INPUT_DOWN) == 1 ){
		m_select = (m_select+1)%NAME_LIST_SIZE;
	}
	if( GetStateKey(KEY_INPUT_UP) == 1 ){
		m_select = (m_select+NAME_LIST_SIZE-1)%NAME_LIST_SIZE;
	}
	if( GetStateKey(KEY_INPUT_RETURN) == 1 ){
		switch( m_select ){
		case 0:
			scene_manager.setNextScene( LOCAL_MATCH_TOKOPUYO );
			break;
		case 1:
			scene_manager.setNextScene( LOCAL_MATCH_AI );
			break;
		case 2:
			break;
		default:
			return -1;
		}
		
	}
	return 0;
}

void SceneLocalSelect::draw(){   // �`�� �X�V��ɖ���Ă�
	const int C0 = GetColor(255,0,0);
	const int C1 = GetColor(128,0,0);
	int color;
	for( int i=0; i<NAME_LIST_SIZE; ++i ){
		color = (m_select==i) ? C0 : C1;
		DrawFormatString( 80,100+20*i,color,NAME_LIST[i] );
	}
}

void SceneLocalSelect::fin(){    // �I������
}