#include "SceneTitle.h"
#include "SceneManager.h"
#include "DxLib.h"
#include "input.h"

static int m_select;
extern SceneManager scene_manager;

void SceneTitleInit(){   // ������ �N�����ɌĂ�
	m_select = 0;
}

int  SceneTitleUpdate(){ // �X�V ���t���[���Ă�
	if( GetStateKey(KEY_INPUT_DOWN) == 1 ){
		m_select = (m_select+1)%2;
	}
	if( GetStateKey(KEY_INPUT_UP) == 1 ){
		m_select = (m_select+1)%2;
	}
	if( GetStateKey(KEY_INPUT_RETURN) == 1 ){
		switch( m_select ){
		case 0:
			scene_manager.setNextScene( NETWORK_PLAY );
			break;
		case 1:
			return -1;
		}
		
	}
	return 0;
}
void SceneTitleDraw(){   // �`�� �X�V��ɖ���Ă�
	DrawFormatString( 60,60,GetColor(255,255,255),"PuyoSimuVSClient" );
	const int C0 = GetColor(255,0,0);
	const int C1 = GetColor(128,0,0);
	int color;
	color = (m_select==0) ? C0 : C1;
	DrawFormatString( 80,100,color,"�T�[�o�ɐڑ�" );
	color = (m_select==1) ? C0 : C1;
	DrawFormatString( 80,120,color,"�I��" );
}
void SceneTitleFin(){    // �I������
}