#include "SceneGame.h"
#include "DxLib.h"
#include "input.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
//#include <winsock2.h>

static int m_flame; // �o�߃t���[����

static SOCKET m_socket;
static Game m_game;
static int m_control_x; // ���쒆�c���̎�x�ʒu(1-6)
static int m_control_y; // ���쒆�c���̎�y�ʒu(1-13)
static int m_control_dir; // ���쒆�c���̉�]��(0-3)
static bool m_decision_flag; // ������m�肵�����ǂ����̃t���O

static bool m_network_vs_flag; // �ʐM�ΐ풆���ǂ����̃t���O

static bool m_ready_flag; // �����J�n�O��ready�𑗐M�������ǂ����̃t���O
/*----------------------------------------------------------------------*/
//      ������
/*----------------------------------------------------------------------*/
void loadConfig( string &hostname, int &port, string &nickname ){
	cout<<"setting.conf�̓ǂݍ���"<<endl;
	ifstream ifs( "setting.conf" );
	if( !ifs ){ cout<<"setting.conf �̃I�[�v���Ɏ��s"<<endl; return;}
	ifs >> hostname >> port >> nickname;
}
void SceneGameInit(){

	string hostname = "localhost";
	string nickname = "NO_NAME";
	int  port = 2424;
	loadConfig( hostname, port, nickname );

	cout<<"�T�[�o�ɐڑ� "<<hostname<<":"<<port<<endl;

	WSADATA wsaData;

	struct sockaddr_in addr;

	WSAStartup(MAKEWORD(2,0), &wsaData);

	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(hostname.c_str());
	if (addr.sin_addr.S_un.S_addr == 0xffffffff) {
		struct hostent *host;

		host = gethostbyname(hostname.c_str());
		if (host == NULL) {
			printf("error:gethostbyname(%s)\n",hostname);
			return;
		}
		addr.sin_addr.S_un.S_addr =
			*(unsigned int *)host->h_addr_list[0];
	}

	int r = connect(m_socket, (struct sockaddr *)&addr, sizeof(addr));
	if( r==-1 ){
		cout<<"�T�[�o�ւ̐ڑ������s"<<endl; return;
	}
	cout<<"�T�[�o�ւ̐ڑ�������"<<endl;
	u_long val=1;
	ioctlsocket(m_socket, FIONBIO, &val); // �m���u���b�L���O�ݒ�

	char buf[64] = {1,nickname.size()};
	for( size_t i=0; i<nickname.size(); ++i ) buf[2+i] = nickname[i];
	buf[2+nickname.size()] = 2;
	buf[3+nickname.size()] = 0;
	send(m_socket, buf, 4+nickname.size(), 0);
	cout<<"�j�b�N�l�[���𑗐M:"<<nickname<<endl;
	cout<<"�����_���}�b�`�ɎQ���\���𑗐M"<<endl;

	m_control_x = 3;
	m_control_y = 13;
	m_control_dir = 0;

	m_network_vs_flag = false;

	m_flame = 0;
	m_decision_flag = true;

	m_ready_flag = false;
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
void processRecv();
void sendReady(){
	char buf[] = {3,0};
	send(m_socket, buf, sizeof(buf), 0);
	cout<<"ready�𑗐M"<<endl;
}
void sendAction(){
	Action act = m_game.getAction(1);
	char buf[] = {4,3,act.id,act.pos,act.dir};
	send(m_socket, buf, sizeof(buf), 0);
	printf("action�𑗐M(%d,%d,%d)\n",act.id,act.pos,act.dir);
}
bool checkControlFlag(){
	return m_game.getAction(1).id==-1;
}
void moveLeft(){
	if( m_control_x<=1 ) return;
	if( m_control_x==2 && m_control_dir==3 ) return;
	m_control_x--;
}
void moveRight(){
	if( m_control_x>=6 ) return;
	if( m_control_x==5 && m_control_dir==1 ) return;
	m_control_x++;
}
void rotateRight(){
	if( m_control_x==1 && m_control_dir==2 ) return;
	if( m_control_x==6 && m_control_dir==0 ) return;
	m_control_dir = (m_control_dir+1)%4;
}
void rotateLeft(){
	if( m_control_x==1 && m_control_dir==0 ) return;
	if( m_control_x==6 && m_control_dir==2 ) return;
	m_control_dir = (m_control_dir+3)%4;
}
void SceneGameUpdate(){
	++m_flame;
	processRecv();

	// �ʐM�ΐ�J�n�O�̏���
	if( !m_network_vs_flag && !m_ready_flag ){
		if( GetStateKey(KEY_INPUT_RETURN) == 1 ){
			sendReady();
		}
		m_ready_flag = true;
		return;
	} 
	// �ȉ��A�ʐM�ΐ�J�n��̏���

	if( m_decision_flag ) return; // ����m���͓��͂��󂯕t���Ȃ�

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
			sendAction();
			sendReady();
			m_decision_flag = true;
		}
	}

}

/*----------------------------------------------------------------------*/
//      �`��
/*----------------------------------------------------------------------*/
static void drawPuyo( int x, int y, int w, int h, int puyo ){
	int color = 0x0;
	switch( puyo ){
	case 0 : return;
	case 1 : color = 0xff0000; break;
	case 2 : color = 0x00ff00; break;
	case 3 : color = 0x0000ff; break;
	case 4 : color = 0xffff00; break;
	case 9 : color = 0xffffff; break;
	}
	DrawOval( x, y, w/2, h/2, color, true );
}
const int CELL_W = 30;
const int CELL_H = 30;
const int LEFT = 100;
const int TOP  = 50;
const int GAP  = 270; // 1p��2p�̊Ԋu
void doDrawField( int left, int top, Field field ){
	for(int x=1;  x<=6; ++x ){
		for(int y=13; y>=1; --y ){
			const int SX = left+(x-1) *CELL_W;
			const int SY = top +(13-y)*CELL_H;
			// �w�i�̕`��
			const int CELL_COLOR = (y==13)?0x444444:0x888888;
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, CELL_COLOR, true );
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0x222222, false );
			// �����_�̕`��
			if( x==3 && y==12 )
				DrawLine( SX+4, SY+4, SX+CELL_W-4, SY+CELL_H-4, 0xff0000, 3 ),
				DrawLine( SX+4, SY+CELL_H-4, SX+CELL_W-4, SY+4, 0xff0000, 3 );
			// �t�B�[���h�Ղ�̕`��
			int puyo = field.get(x,y);
			drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, puyo );
		}}
}
void drawField( Field field, int player_id ){
	doDrawField( LEFT+(player_id==2?GAP:0), TOP, field );
}
void drawTumo( int x, int y, int dir, Tumo tumo, int player_id ){
	// TODO:���ꂢ�ɏ���
	int sx = x+(dir==1?1:(dir==3?-1:0));
	int sy = y+(dir==0?1:(dir==2?-1:0));
	if( checkControlFlag() && m_decision_flag==false && player_id==1 && ((m_flame%60)>30) ){
		drawPuyo( LEFT+(int)(( x-0.5)*CELL_W)+(player_id==2?GAP:0), TOP+(int)(( 13-y+0.5)*CELL_H), CELL_W, CELL_H, 9 );
		drawPuyo( LEFT+(int)((sx-0.5)*CELL_W)+(player_id==2?GAP:0), TOP+(int)((13-sy+0.5)*CELL_H), CELL_W, CELL_H, 9 );
	}
	if( m_network_vs_flag && m_game.getAction(2).id==-1 && player_id==2 && ((m_flame%60)>30) ){
		drawPuyo( LEFT+(int)(( x-0.5)*CELL_W)+(player_id==2?GAP:0), TOP+(int)(( 13-y+0.5)*CELL_H), CELL_W, CELL_H, 9 );
		drawPuyo( LEFT+(int)((sx-0.5)*CELL_W)+(player_id==2?GAP:0), TOP+(int)((13-sy+0.5)*CELL_H), CELL_W, CELL_H, 9 );
	}
	drawPuyo( LEFT+(int)(( x-0.5)*CELL_W)+(player_id==2?GAP:0), TOP+(int)(( 13-y+0.5)*CELL_H), CELL_W-1, CELL_H-1, tumo.first );
	drawPuyo( LEFT+(int)((sx-0.5)*CELL_W)+(player_id==2?GAP:0), TOP+(int)((13-sy+0.5)*CELL_H), CELL_W-1, CELL_H-1, tumo.second );
}
void drawNext( Tumo next1p1, Tumo next2p1, Tumo next1p2, Tumo next2p2 ){
	// 1p
	drawPuyo( 300,  80       , CELL_W-1, CELL_H-1, next1p1.second );
	drawPuyo( 300,  80+CELL_H, CELL_W-1, CELL_H-1, next1p1.first  );
	drawPuyo( 308, 145       , CELL_W-4, CELL_H-4, next2p1.second );
	drawPuyo( 308, 145+CELL_H, CELL_W-4, CELL_H-4, next2p1.first  );
	// 2p
	drawPuyo( 350,  80       , CELL_W-1, CELL_H-1, next1p2.second );
	drawPuyo( 350,  81+CELL_H, CELL_W-1, CELL_H-1, next1p2.first  );
	drawPuyo( 342, 145       , CELL_W-4, CELL_H-4, next2p2.second );
	drawPuyo( 342, 145+CELL_H, CELL_W-4, CELL_H-4, next2p2.first  );
}
void drawInfo(){
	// 1p
	DrawFormatString( 270,10,GetColor(255,255,255),"%d:%d", m_game.getOjamaStock(1), m_game.getOjamaNotice(1) );
	// 2p
	DrawFormatString( 340,10,GetColor(255,255,255),"%d:%d", m_game.getOjamaStock(2), m_game.getOjamaNotice(2) );
}
void drawScore(){
	// 1p
	DrawFormatString( 100,445,GetColor(255,255,255),"score:%d", m_game.getScore(1) );
	// 2p
	DrawFormatString( 360,445,GetColor(255,255,255),"score:%d", m_game.getScore(2) );
}
void SceneGameDraw(){
	drawField( m_game.getMyField(),    1 );
	drawField( m_game.getEnemyField(), 2 );
	drawTumo( m_control_x, m_control_y, m_control_dir, m_game.getNextTumo(0,1), 1 );
	drawTumo(           3,          13,             0, m_game.getNextTumo(0,2), 2 );
	drawNext( m_game.getNextTumo(1,1), m_game.getNextTumo(2,1),
		m_game.getNextTumo(1,2), m_game.getNextTumo(2,2) );
	drawInfo();
	drawScore();
}

/*----------------------------------------------------------------------*/
//      �I������
/*----------------------------------------------------------------------*/
void SceneGameFin(){
	char buf[] = {6,0};
	send(m_socket, buf, sizeof(buf), 0);
	cout<<"fin�𑗐M"<<endl;

	closesocket(m_socket);
	WSACleanup();
}



//------------------------------------------------------------------------
// ��M�f�[�^�̏������낢��
enum eRecvPhase{ HEADER, ENEMY_NAME, NEXT_TABLE, ENEMY_ACTION };
static char       recv_buf[1024];
static int        next_recv_size  = 2;
static eRecvPhase next_recv_phase = HEADER;

// socket����f�[�^��ǂݏo��
// next_recv_size�܂œǂݏo���I�������true��Ԃ�
bool doRecv(){
	static int recv_len = 0; // ���݂܂łɓǂݏo���I����Ă��钷��
	// next_recv_size�܂�recv_buf�ɂ��܂�܂�1byte���ǂݏo��������
	while( next_recv_size>recv_len ){
		if( recv(m_socket, recv_buf+recv_len, 1, 0)<=0 ) break;
		++recv_len;
	}
	if(next_recv_size<=recv_len){
		recv_len = 0;
		return true;
	}
	return false;
}
void matchingNotice();
void setEnemyName( char buf[], int len );
void beginTurn();
void setNextTable( char buf[] );
void setEnemyAction( char buf[] );
void processRecv(){
	while(true){
		if( !doRecv() ) return;
		switch( next_recv_phase ){
		case HEADER:
			if( recv_buf[0]==1 ){ // �}�b�`���O�����ʒm
				matchingNotice();
				break;
			}
			if( recv_buf[0]==2 ){ // �ΐ푊��̖��O�ʒm
				next_recv_phase = ENEMY_NAME;
				next_recv_size  = recv_buf[1];
				break;
			}
			if( recv_buf[0]==3 ){ // next�e�[�u���̒ʒm
				next_recv_phase = NEXT_TABLE;
				next_recv_size  = 256;
				break;
			}
			if( recv_buf[0]==4 ){ // �^�[���J�n�ʒm
				beginTurn();
				break;
			}
			if( recv_buf[0]==5 ){ // ����̍s���ʒm
				next_recv_phase = ENEMY_ACTION;
				next_recv_size  = 3;
				break;
			}
			printf("erro:�T�[�o���疢��`�̃��b�Z�[�W����M\n");
			break;
		case ENEMY_NAME:
			setEnemyName( recv_buf, next_recv_size );
			next_recv_phase = HEADER;
			next_recv_size  = 2;
			break;
		case NEXT_TABLE:
			setNextTable( recv_buf );
			next_recv_phase = HEADER;
			next_recv_size  = 2;
			break;
		case ENEMY_ACTION:
			setEnemyAction( recv_buf );
			next_recv_phase = HEADER;
			next_recv_size  = 2;
			break;
		default:
			printf("error:����`�̎�Mphase�ɓ��B\n");
		}
	}
}


//------------------------------------------------------------------------
// ��M�f�[�^�̏������낢��
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
	m_game.goNextStep();
	if( m_game.getStatus(1)==0 ) m_decision_flag = false;
	if( m_game.getStatus(1)==1 ){
		m_game.setAction(Action(0,0,0),1);
		sendAction();
		sendReady();
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

