#include "SceneNetworkPlay.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "input.h"
#include <iostream>
#include "OldAI.h"
#include "UI.h"

using namespace std;

extern SceneManager scene_manager;

void SceneNetworkPlay::init( PARAM param ){   // 初期化 起動時に呼ぶ
	m_gm.init();
	m_gm.start();
	// setting.ini 読み込み
	m_setting.load();
	// Connection関係
	string hostname = m_setting.getHostName();
	string nickname = m_setting.getNickName();
	int    port     = m_setting.getPort();
	cout<<"サーバに接続 "<<hostname<<":"<<port<<endl;
	m_connection.connect( (char *)hostname.c_str(), port );
	cout<<"ニックネームを送信:"<<nickname<<endl;
	m_connection.sendNickname( (char *)nickname.c_str(), nickname.size() );
	cout<<"ランダムマッチに参加申請を送信"<<endl;
	m_connection.sendJoin();
	// 初期化いろいろ
	m_action_flag = false;
	m_begin_turn_first = true;
}

int  SceneNetworkPlay::update(){ // 更新 毎フレーム呼ぶ
	processRecv();
	m_gm.update();

	if( GetStateKey(KEY_INPUT_ESCAPE) == 1 ){
		scene_manager.setNextScene( TITLE );
		return 0;
	}

	if( !m_action_flag ) return 0;

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
		m_connection.sendAction( m_gm.getAction( 1 ) );
		m_connection.sendReady();
		m_action_flag = false;
	}

	if( GetStateKey(KEY_INPUT_P) == 1 ){
		m_gm.setAction(1,0);
		m_connection.sendAction( m_gm.getAction( 1 ) );
		m_connection.sendReady();
		m_action_flag = false;
	}

	return 0;
}

void SceneNetworkPlay::draw(){   // 描画 更新後に毎回呼ぶ
	m_gm.draw();
}

void SceneNetworkPlay::fin(){    // 終了処理
	m_gm.fin();
}

void SceneNetworkPlay::processRecv(){
	while(true){
		if( !m_connection.doRecv() ) return;
		char *recv_buf = m_connection.getRecvBuffer();
		int  recv_size = m_connection.getRecvSize();
		switch( m_connection.getRecvPhase() ){
		case HEADER:
			if( recv_buf[0]==1 ){ // マッチング成立通知
				matchingNotice();
			}
			if( recv_buf[0]==4 ){ // ターン開始通知
				beginTurn();
			}
			break;
		case ENEMY_NAME:
			setEnemyName( recv_buf, recv_size );
			break;
		case NEXT_TABLE:
			setNextTable( recv_buf );
			break;
		case ENEMY_ACTION:
			setEnemyAction( recv_buf );
			break;
		default:
			printf("error:未定義の受信phaseに到達\n");
		}
		m_connection.forwardRecvPhase();
	}
}
void SceneNetworkPlay::matchingNotice(){
	printf("matching成立通知受信\n");
	m_connection.sendReady();
	m_action_flag = true;
	UI::gi().playMatch();
}
void SceneNetworkPlay::setEnemyName( char buf[], int len ){
	char name[64] = {0};
	for( int i=0; i<len; ++i ) name[i] = buf[i];
	printf("対戦相手名受信:%s\n",name);
}
void SceneNetworkPlay::setNextTable( char buf[] ){
	printf("Nextテーブル受信\n");
	Next next(buf);
	m_gm.setNext( next );
}
void SceneNetworkPlay::beginTurn(){
	printf("ターン開始通知受信\n");
	m_action_flag = true;
	if( m_begin_turn_first ){
		m_begin_turn_first = false;
		return;
	}
	m_gm.setAction(2,m_enemy_action);
	m_gm.goNext();
	if( m_gm.isChain(1) ){ // 連鎖判定
		m_action_flag = false;
		m_gm.setAction(1,Action(0,0,0));
		m_connection.sendAction(Action(0,0,0));
		m_connection.sendReady();
	}
}
void SceneNetworkPlay::setEnemyAction( char buf[] ){
	printf("enemy行動通知受信\n");
	m_enemy_action = Action( buf[0], buf[1], buf[2] );
	UI::gi().playNotice();
}
