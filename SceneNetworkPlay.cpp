#include "SceneNetworkPlay.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "input.h"
#include <iostream>
#include "OldAI.h"
#include "UI.h"

using namespace std;

extern SceneManager scene_manager;

void SceneNetworkPlay::init( PARAM param ){   // ������ �N�����ɌĂ�
	m_gm.init();
	m_gm.start();
	// setting.ini �ǂݍ���
	m_setting.load();
	// Connection�֌W
	string hostname = m_setting.getHostName();
	string nickname = m_setting.getNickName();
	int    port     = m_setting.getPort();
	cout<<"�T�[�o�ɐڑ� "<<hostname<<":"<<port<<endl;
	m_connection.connect( (char *)hostname.c_str(), port );
	cout<<"�j�b�N�l�[���𑗐M:"<<nickname<<endl;
	m_connection.sendNickname( (char *)nickname.c_str(), nickname.size() );
	cout<<"�����_���}�b�`�ɎQ���\���𑗐M"<<endl;
	m_connection.sendJoin();
	// ���������낢��
	m_action_flag = false;
	m_begin_turn_first = true;
}

int  SceneNetworkPlay::update(){ // �X�V ���t���[���Ă�
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

void SceneNetworkPlay::draw(){   // �`�� �X�V��ɖ���Ă�
	m_gm.draw();
}

void SceneNetworkPlay::fin(){    // �I������
	m_gm.fin();
}

void SceneNetworkPlay::processRecv(){
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
void SceneNetworkPlay::matchingNotice(){
	printf("matching�����ʒm��M\n");
	m_connection.sendReady();
	m_action_flag = true;
	UI::gi().playMatch();
}
void SceneNetworkPlay::setEnemyName( char buf[], int len ){
	char name[64] = {0};
	for( int i=0; i<len; ++i ) name[i] = buf[i];
	printf("�ΐ푊�薼��M:%s\n",name);
}
void SceneNetworkPlay::setNextTable( char buf[] ){
	printf("Next�e�[�u����M\n");
	Next next(buf);
	m_gm.setNext( next );
}
void SceneNetworkPlay::beginTurn(){
	printf("�^�[���J�n�ʒm��M\n");
	m_action_flag = true;
	if( m_begin_turn_first ){
		m_begin_turn_first = false;
		return;
	}
	m_gm.setAction(2,m_enemy_action);
	m_gm.goNext();
	if( m_gm.isChain(1) ){ // �A������
		m_action_flag = false;
		m_gm.setAction(1,Action(0,0,0));
		m_connection.sendAction(Action(0,0,0));
		m_connection.sendReady();
	}
}
void SceneNetworkPlay::setEnemyAction( char buf[] ){
	printf("enemy�s���ʒm��M\n");
	m_enemy_action = Action( buf[0], buf[1], buf[2] );
	UI::gi().playNotice();
}
