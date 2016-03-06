#include "SceneNetworkPlay.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "input.h"
#include "Game.h"
#include "UI.h"
#include "Connection.h"
#include "AnnimationManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
using namespace std;
//#include <winsock2.h>

extern SceneManager scene_manager;

static Game m_game;
static Connection m_connection;
static UI m_ui;
static int m_control_x; // 操作中ツモの軸x位置(1-6)
static int m_control_y; // 操作中ツモの軸y位置(1-13)
static int m_control_dir; // 操作中ツモの回転数(0-3)
static bool m_decision_flag; // 操作を確定したかどうかのフラグ

static bool m_network_vs_flag; // 通信対戦中かどうかのフラグ

static bool m_ready_flag; // 試合開始前にreadyを送信したかどうかのフラグ



#define m_myfield ( m_game.getMyField() )
#define m_sub_x ( m_control_x + (m_control_dir==1?1:(m_control_dir==3?-1:0)) )
#define m_sub_y ( m_control_y + (m_control_dir==0?1:(m_control_dir==2?-1:0)) )
/*----------------------------------------------------------------------*/
//      初期化
/*----------------------------------------------------------------------*/
void loadConfig( string &hostname, int &port, string &nickname ){
	cout<<"setting.confの読み込み"<<endl;
	ifstream ifs( "setting.conf" );
	if( !ifs ){ cout<<"setting.conf のオープンに失敗"<<endl; return;}
	ifs >> hostname >> port >> nickname;
}
void SceneNetworkPlayInit(){
	// 画像データ読み込み
	m_ui.load();
	
	// setting.conf 読み込み
	string hostname = "localhost";
	string nickname = "NO_NAME";
	int  port = 2424;
	loadConfig( hostname, port, nickname );

	// Connection関係
	cout<<"サーバに接続 "<<hostname<<":"<<port<<endl;
	m_connection.connect( (char *)hostname.c_str(), port );
	cout<<"ニックネームを送信:"<<nickname<<endl;
	m_connection.sendNickname( (char *)nickname.c_str(), nickname.size() );
	cout<<"ランダムマッチに参加申請を送信"<<endl;
	m_connection.sendJoin();

	// オブジェクト初期化
	m_control_x = 3;
	m_control_y = 12;
	m_control_dir = 0;
	m_network_vs_flag = false;
	m_decision_flag = true;
	m_ready_flag = false;
	m_game.init();
}

void beginNetworkVS(){
	if( m_network_vs_flag ) return;
	m_network_vs_flag = true;
	m_decision_flag = false;
	m_game.init();
}

/*----------------------------------------------------------------------*/
//      更新
/*----------------------------------------------------------------------*/
//------------------------------------------------------------------------
void matchingNotice(){
	printf("matching成立通知受信\n");
}
void setEnemyName( char buf[], int len ){
	char name[64] = {0};
	for( int i=0; i<len; ++i ) name[i] = buf[i];
	printf("対戦相手名受信:%s\n",name);
}
void setNextTable( char buf[] ){
	printf("Nextテーブル受信\n");
	Next next(buf);
	m_game.setNext( next );
}
void beginTurn(){
	printf("ターン開始通知受信\n");
	if( !m_network_vs_flag ){
		beginNetworkVS();
		return;
	}
	// 設置アニメーション
	for( int i=0; i<2; ++i ){
		//Action act = m_game.getHistory(1+i,0);
		
		//if( act.id!=1 ) continue;
		//printf("%d:(%d,%d,%d)\n",i+1,act.id,act.pos,act.dir);
		//fallAnnimation *anime = new fallAnnimation( act.pos*30, 50, 500, GetColor(255,0,0) );
		//m_annimation_manager.add( anime );
	}

	m_game.goNextStep();
	if( m_game.getStatus(1)==0 ) m_decision_flag = false;
	if( m_game.getStatus(1)==1 ){
		m_game.setAction(Action(0,0,0),1);
		m_connection.sendAction( m_game.getAction(1) );
		m_connection.sendReady();
		m_decision_flag = true;
	}
	m_control_x = 3;
	m_control_y = 13;
	m_control_dir = 0;
}
void setEnemyAction( char buf[] ){
	printf("enemy行動通知受信\n");
	Action act( buf[0], buf[1], buf[2] );
	m_game.setAction(act,2);
}
void processRecv(){
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
bool checkControlFlag(){
	return m_game.getAction(1).id==-1;
}
void moveLeft(){
	--m_control_x;
	// 左に障害物
	if( m_myfield.get(m_control_x,m_control_y)!=0 ) ++m_control_x;
	if( m_myfield.get(    m_sub_x,    m_sub_y)!=0 ) ++m_control_x;
}
void moveRight(){
	++m_control_x;
	// 右に障害物
	if( m_myfield.get(m_control_x,m_control_y)!=0 ) --m_control_x;
	if( m_myfield.get(    m_sub_x,    m_sub_y)!=0 ) --m_control_x;
}
void rotateRight(){
	// 下に障害物
	if( m_control_dir==1 && m_myfield.get(m_control_x,12)!=0 ) return;
	// 左右に障害物
	if( m_myfield.get(m_control_x-1,13)!=0 && m_myfield.get(m_control_x+1,13)!=0 ) return;
	// 左に障害物
	if( m_control_dir==2 && m_myfield.get(m_control_x-1,13)!=0 ) ++m_control_x;
	// 右に障害物
	if( m_control_dir==0 && m_myfield.get(m_control_x+1,13)!=0 ) --m_control_x;

	m_control_dir = (m_control_dir+1)%4;
}
void rotateLeft(){
	// 下に障害物
	if( m_control_dir==3 && m_myfield.get(m_control_x,12)!=0 ) return;
	// 左右に障害物
	if( m_myfield.get(m_control_x-1,13)!=0 && m_myfield.get(m_control_x+1,13)!=0 ) return;
	// 左に障害物
	if( m_control_dir==0 && m_myfield.get(m_control_x-1,13)!=0 ) ++m_control_x;
	// 右に障害物
	if( m_control_dir==2 && m_myfield.get(m_control_x+1,13)!=0 ) --m_control_x;

	m_control_dir = (m_control_dir+3)%4;
}
int SceneNetworkPlayUpdate(){
	processRecv();
	// 
	if( GetStateKey(KEY_INPUT_ESCAPE) == 1 ){
		scene_manager.setNextScene( TITLE );
		return 0;
	}
	if( GetStateKey(KEY_INPUT_D) == 1 ){
		m_game.debug();
	}

	// 通信対戦開始前の処理
	if( !m_network_vs_flag && !m_ready_flag ){
		if( GetStateKey(KEY_INPUT_RETURN) == 1 ){
			m_connection.sendReady();
			m_ready_flag = true;
		}
		return 0;
	} 
	// 以下、通信対戦開始後の処理

	if( m_decision_flag ) return 0; // 操作確定後は入力を受け付けない

	if( GetStateKey(KEY_INPUT_LEFT) == 1 ){
		if( checkControlFlag() )
			moveLeft();
	}
	if( GetStateKey(KEY_INPUT_RIGHT) == 1 ){
		if( checkControlFlag() )
			moveRight();
	}
	if( GetStateKey(KEY_INPUT_Z) == 1 ){
		if( checkControlFlag() )
			rotateLeft();
	}
	if( GetStateKey(KEY_INPUT_X) == 1 ){
		if( checkControlFlag() )
			rotateRight();
	}
	if( GetStateKey(KEY_INPUT_DOWN) == 1 ){
		if( checkControlFlag() ){
			m_game.setAction( Action(1,m_control_x,m_control_dir),1 );
			m_connection.sendAction( m_game.getAction(1) );
			m_connection.sendReady();
			m_decision_flag = true;
			return 0;
		}
	}
	if( GetStateKey(KEY_INPUT_P) == 1 ){
		if( checkControlFlag() && m_game.getHistory(1,1).id!=0 ){
			m_game.setAction( Action(0,0,0),1 );
			m_connection.sendAction( m_game.getAction(1) );
			m_connection.sendReady();
			m_decision_flag = true;
			return 0;
		}
	}
	return 0;
}

/*----------------------------------------------------------------------*/
//      描画
/*----------------------------------------------------------------------*/
void SceneNetworkPlayDraw(){
	m_ui.drawBackGround();
	m_ui.drawField( m_game.getMyField(),    1 );
	m_ui.drawField( m_game.getEnemyField(), 2 );
	m_ui.drawTumo( m_control_x, m_control_y, m_control_dir, m_game.getNextTumo(0,1), 1 );
	m_ui.drawTumo(           3,          12,             0, m_game.getNextTumo(0,2), 2 );
	m_ui.drawNext( m_game.getNextTumo(1,1), m_game.getNextTumo(2,1), 1 );
	m_ui.drawNext( m_game.getNextTumo(1,2), m_game.getNextTumo(2,2), 2 );
	m_ui.drawScore( m_game.getScore(1), 1 );
	m_ui.drawScore( m_game.getScore(2), 2 );
	m_ui.drawFrame();
	m_ui.drawOjamaNotice( m_game.getOjamaNotice(1) + m_game.getOjamaStock(1), 1 );
	m_ui.drawOjamaNotice( m_game.getOjamaNotice(2) + m_game.getOjamaStock(2), 2 );
	
	m_ui.drawDebugInfo( m_game );
}

/*----------------------------------------------------------------------*/
//      終了処理
/*----------------------------------------------------------------------*/
void SceneNetworkPlayFin(){
	m_connection.sendFin();
	m_connection.close();
	m_ui.free();
}




