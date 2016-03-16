#include "SceneTitle.h"
#include "SceneManager.h"
#include "AnnimationManager.h"
#include "DxLib.h"
#include "input.h"
#include <iostream>
using namespace std;

extern SceneManager scene_manager;
const int SELECT_SIZE = 3;

void SceneTitle::init( PARAM param ){
	m_select = 0;
}

int  SceneTitle::update(){
	if( GetStateKey(KEY_INPUT_DOWN) == 1 ){
		m_select = (m_select+1)%SELECT_SIZE;
	}
	if( GetStateKey(KEY_INPUT_UP) == 1 ){
		m_select = (m_select+SELECT_SIZE-1)%SELECT_SIZE;
	}
	if( GetStateKey(KEY_INPUT_RETURN) == 1 ){
		switch( m_select ){
		case 0:
			scene_manager.setNextScene( LOCAL_SELECT );
			break;
		case 1:
			scene_manager.setNextScene( NETWORK_PLAY );
			break;
		case 2:
			return -1;
		}
		
	}
	return 0;
}

void SceneTitle::draw(){
	DrawFormatString( 60,60,GetColor(255,255,255),"PuyoSimuVSClient" );
	const int C0 = GetColor(255,0,0);
	const int C1 = GetColor(128,0,0);
	int color;
	color = (m_select==0) ? C0 : C1;
	DrawFormatString( 80,100,color,"ローカルで対戦" );
	color = (m_select==1) ? C0 : C1;
	DrawFormatString( 80,120,color,"サーバに接続" );
	color = (m_select==2) ? C0 : C1;
	DrawFormatString( 80,140,color,"終了" );
}

void SceneTitle::fin(){

}
