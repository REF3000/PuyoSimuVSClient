#include "Connection.h"
#include <iostream>
using namespace std;

Connection::Connection(){
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,0), &wsaData);

	recv_size  = 0;
	recv_next_size  = 2;
	recv_phase = HEADER;
}

int Connection::connect( char *hostname, int port ){
	struct sockaddr_in addr;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(hostname);
	if (addr.sin_addr.S_un.S_addr == 0xffffffff) {
		struct hostent *host;

		host = gethostbyname( hostname );
		if (host == NULL) {
			printf("error:gethostbyname(%s)\n",hostname);
			return -1;
		}
		addr.sin_addr.S_un.S_addr =
			*(unsigned int *)host->h_addr_list[0];
	}

	int r = ::connect(m_socket, (struct sockaddr *)&addr, sizeof(addr));
	if( r==-1 ){
		cout<<"�T�[�o�ւ̐ڑ������s"<<endl; return -1;
	}
	cout<<"�T�[�o�ւ̐ڑ�������"<<endl;
	u_long val=1;
	ioctlsocket(m_socket, FIONBIO, &val); // �m���u���b�L���O�ݒ�

	return 0;
}

void Connection::close(){
	closesocket(m_socket);
	//WSACleanup(); ������Ă񂶂�_���炵�� TODO:�I����1��ĂԌ`�ɂ���

}

int Connection::send( char *data, int size ){
	return ::send(m_socket, data, size, 0);
}


int Connection::recv( char *buf, int len ){
	return ::recv(m_socket, buf, len, 0);
}


int Connection::sendNickname( char *name, int len ){
	char buf[64] = {1,len};
	for( int i=0; i<len; ++i ) buf[2+i] = name[i];
	return send( buf, 2+len );
}

int Connection::sendJoin(){
	char buf[2] = {2,0};
	return send( buf, 2 );
}

int Connection::sendReady(){
	cout<<"ready�𑗐M"<<endl;
	char buf[2] = {3,0};
	return send( buf, 2 );
}

int Connection::sendAction( Action action ){
	printf("action�𑗐M(%d,%d,%d)\n",action.id,action.pos,action.dir);
	char buf[] = {4,3,action.id,action.pos,action.dir};
	return send( buf, sizeof(buf) );
}

int Connection::sendFin(){
	cout<<"fin�𑗐M"<<endl;
	char buf[] = {6,0};
	return send( buf, 2 );
}

// socket����f�[�^��ǂݏo��
// next_recv_size�܂œǂݏo���I�������true��Ԃ�
bool Connection::doRecv(){
	// next_recv_size�܂�recv_buf�ɂ��܂�܂�1byte���ǂݏo��������
	while( recv_next_size>recv_size ){
		if( recv(recv_buf+recv_size,1)<=0 ) break;
		++recv_size;
	}
	if(recv_next_size<=recv_size){
		return true;
	}
	return false;
}

int Connection::getRecvSize(){
	return recv_size;
}

eRecvPhase Connection::getRecvPhase(){
	return recv_phase;
}

char* Connection::getRecvBuffer(){
	return recv_buf;
}

void Connection::forwardRecvPhase(){
	recv_size = 0;
	switch( recv_phase ){
	case HEADER:
		switch( recv_buf[0] ){
		case 1:
			return;
		case 2:
			recv_phase = ENEMY_NAME;
			recv_next_size  = recv_buf[1];
			return;
		case 3:
			recv_phase = NEXT_TABLE;
			recv_next_size  = 256;
			return;
		case 4:
			return;
		case 5:
			recv_phase = ENEMY_ACTION;
			recv_next_size  = 3;
			return;
		default:
			printf("����`�̃��b�Z�[�W\n");
		}
		return;
	case ENEMY_NAME:
	case NEXT_TABLE:
	case ENEMY_ACTION:
		recv_phase = HEADER;
		recv_next_size = 2;
		return;
	}
	
}