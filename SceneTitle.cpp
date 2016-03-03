#include "SceneTitle.h"
#include "SceneManager.h"
#include "AnnimationManager.h"
#include "DxLib.h"
#include "input.h"
#include <iostream>
using namespace std;
static int m_select;
extern SceneManager scene_manager;

static AnnimationManager m_annimation_manager;

void SceneTitleInit(){   // ������ �N�����ɌĂ�
	m_select = 0;
}

int  SceneTitleUpdate(){ // �X�V ���t���[���Ă�
	m_annimation_manager.update();

	if( GetStateKey(KEY_INPUT_D) == 1 ){
		sampleAnnimation *obj = new sampleAnnimation( GetRand(600), GetRand(400) );
		m_annimation_manager.add( obj );
	}
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
	m_annimation_manager.draw();
}
void SceneTitleFin(){    // �I������
}