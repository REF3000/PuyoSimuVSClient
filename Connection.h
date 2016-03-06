#include <Windows.h>
#include "Game.h"

enum eRecvPhase{ HEADER, ENEMY_NAME, NEXT_TABLE, ENEMY_ACTION };

class Connection{

	SOCKET m_socket;

	char       recv_buf[1024];
	int        recv_size;
	int        recv_next_size;
	eRecvPhase recv_phase;

	int  send( char *data, int size );
	int  recv( char *buf, int len );

public:

	Connection();
	int connect( char *host="localhost", int port=2424 );
	void close();


	int sendNickname( char *name, int len );
	int sendJoin();
	int sendReady();
	int sendAction( Action action );
	int sendFin();

	bool doRecv();
	int        getRecvSize();
	char*      getRecvBuffer();
	eRecvPhase getRecvPhase();
	void forwardRecvPhase();
};