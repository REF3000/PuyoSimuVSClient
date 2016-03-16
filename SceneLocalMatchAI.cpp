#include "SceneLocalMatchAI.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "input.h"
#include <iostream>
using namespace std;

extern SceneManager scene_manager;

void SceneLocalMatchAI::init( PARAM param ){   // 初期化 起動時に呼ぶ
	// TODO:paramによってAIの種類を変化させる
	m_gm.init();
	m_gm.start();
}

int  SceneLocalMatchAI::update(){ // 更新 毎フレーム呼ぶ
	m_gm.update();

	if( GetStateKey(KEY_INPUT_ESCAPE) == 1 ){
		scene_manager.setNextScene( LOCAL_SELECT );
		return 0;
	}

	int r = GetStateKey(KEY_INPUT_RIGHT);
	if( r == 1 )      m_gm.moveRight(1);
	if( r>20 && r%2 ) m_gm.moveRight(1);

	int l = GetStateKey(KEY_INPUT_LEFT);
	if( l == 1 )      m_gm.moveLeft(1);
	if( l>20 && l%2 ) m_gm.moveLeft(1);

	if( GetStateKey(KEY_INPUT_X) == 1 ){
		m_gm.turnRight(1);
	}
	
	if( GetStateKey(KEY_INPUT_Z) == 1 ){
		m_gm.turnLeft(1);
	}

	if( GetStateKey(KEY_INPUT_DOWN) == 1 ){
		m_gm.setAction(1,1);
		m_gm.goNext();
	}

	if( GetStateKey(KEY_INPUT_P) == 1 ){
		m_gm.setAction(1,0);
		m_gm.goNext();
	}

	return 0;
}

void SceneLocalMatchAI::draw(){   // 描画 更新後に毎回呼ぶ
	m_gm.draw();
}

void SceneLocalMatchAI::fin(){    // 終了処理
	m_gm.fin();
}