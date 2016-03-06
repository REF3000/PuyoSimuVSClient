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
static int m_control_x; // ���쒆�c���̎�x�ʒu(1-6)
static int m_control_y; // ���쒆�c���̎�y�ʒu(1-13)
static int m_control_dir; // ���쒆�c���̉�]��(0-3)
static bool m_decision_flag; // ������m�肵�����ǂ����̃t���O

static bool m_network_vs_flag; // �ʐM�ΐ풆���ǂ����̃t���O

static bool m_ready_flag; // �����J�n�O��ready�𑗐M�������ǂ����̃t���O



#define m_myfield ( m_game.getMyField() )
#define m_sub_x ( m_control_x + (m_control_dir==1?1:(m_control_dir==3?-1:0)) )
#define m_sub_y ( m_control_y + (m_control_dir==0?1:(m_control_dir==2?-1:0)) )
/*----------------------------------------------------------------------*/
//      ������
/*----------------------------------------------------------------------*/
void loadConfig( string &hostname, int &port, string &nickname ){
	cout<<"setting.conf�̓ǂݍ���"<<endl;
	ifstream ifs( "setting.conf" );
	if( !ifs ){ cout<<"setting.conf �̃I�[�v���Ɏ��s"<<endl; return;}
	ifs >> hostname >> port >> nickname;
}
void SceneNetworkPlayInit(){
	// �摜�f�[�^�ǂݍ���
	m_ui.load();
	
	// setting.conf �ǂݍ���
	string hostname = "localhost";
	string nickname = "NO_NAME";
	int  port = 2424;
	loadConfig( hostname, port, nickname );

	// Connection�֌W
	cout<<"�T�[�o�ɐڑ� "<<hostname<<":"<<port<<endl;
	m_connection.connect( (char *)hostname.c_str(), port );
	cout<<"�j�b�N�l�[���𑗐M:"<<nickname<<endl;
	m_connection.sendNickname( (char *)nickname.c_str(), nickname.size() );
	cout<<"�����_���}�b�`�ɎQ���\���𑗐M"<<endl;
	m_connection.sendJoin();

	// �I�u�W�F�N�g������
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
//      �X�V
/*----------------------------------------------------------------------*/
//------------------------------------------------------------------------
void matchingNotice(){
	printf("matching�����ʒm��M\n");
}
void setEnemyName( char buf[], int len ){
	char name[64] = {0};
	for( int i=0; i<len; ++i ) name[i] = buf[i];
	printf("�ΐ푊�薼��M:%s\n",name);
}
void setNextTable( char buf[] ){
	printf("Next�e�[�u����M\n");
	Next next(buf);
	m_game.setNext( next );
}
void beginTurn(){
	printf("�^�[���J�n�ʒm��M\n");
	if( !m_network_vs_flag ){
		beginNetworkVS();
		return;
	}
	// �ݒu�A�j���[�V����
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
	printf("enemy�s���ʒm��M\n");
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
			if( recv_buf[0]==1 ){ // �}�b�`���O�����ʒm
				matchingNotice();
			}
			if( recv_buf[0]==4 ){ // �^�[���J�n�ʒm
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
			printf("error:����`�̎�Mphase�ɓ��B\n");
		}
		m_connection.forwardRecvPhase();
	}
}
bool checkControlFlag(){
	return m_game.getAction(1).id==-1;
}
void moveLeft(){
	--m_control_x;
	// ���ɏ�Q��
	if( m_myfield.get(m_control_x,m_control_y)!=0 ) ++m_control_x;
	if( m_myfield.get(    m_sub_x,    m_sub_y)!=0 ) ++m_control_x;
}
void moveRight(){
	++m_control_x;
	// �E�ɏ�Q��
	if( m_myfield.get(m_control_x,m_control_y)!=0 ) --m_control_x;
	if( m_myfield.get(    m_sub_x,    m_sub_y)!=0 ) --m_control_x;
}
void rotateRight(){
	// ���ɏ�Q��
	if( m_control_dir==1 && m_myfield.get(m_control_x,12)!=0 ) return;
	// ���E�ɏ�Q��
	if( m_myfield.get(m_control_x-1,13)!=0 && m_myfield.get(m_control_x+1,13)!=0 ) return;
	// ���ɏ�Q��
	if( m_control_dir==2 && m_myfield.get(m_control_x-1,13)!=0 ) ++m_control_x;
	// �E�ɏ�Q��
	if( m_control_dir==0 && m_myfield.get(m_control_x+1,13)!=0 ) --m_control_x;

	m_control_dir = (m_control_dir+1)%4;
}
void rotateLeft(){
	// ���ɏ�Q��
	if( m_control_dir==3 && m_myfield.get(m_control_x,12)!=0 ) return;
	// ���E�ɏ�Q��
	if( m_myfield.get(m_control_x-1,13)!=0 && m_myfield.get(m_control_x+1,13)!=0 ) return;
	// ���ɏ�Q��
	if( m_control_dir==0 && m_myfield.get(m_control_x-1,13)!=0 ) ++m_control_x;
	// �E�ɏ�Q��
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

	// �ʐM�ΐ�J�n�O�̏���
	if( !m_network_vs_flag && !m_ready_flag ){
		if( GetStateKey(KEY_INPUT_RETURN) == 1 ){
			m_connection.sendReady();
			m_ready_flag = true;
		}
		return 0;
	} 
	// �ȉ��A�ʐM�ΐ�J�n��̏���

	if( m_decision_flag ) return 0; // ����m���͓��͂��󂯕t���Ȃ�

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
//      �`��
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
//      �I������
/*----------------------------------------------------------------------*/
void SceneNetworkPlayFin(){
	m_connection.sendFin();
	m_connection.close();
	m_ui.free();
}




