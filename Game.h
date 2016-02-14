#include <utility>
#include <random>
#include <vector>
#include <iostream>
using namespace std;

typedef std::pair<int,int> Tumo;

struct Action{
	int id;
	int pos;
	int dir;
	Action( int id=-1, int pos=0, int dir=0 ){
		this->id = id;
		this->pos = pos;
		this->dir = dir;
	}
};
struct Field{
	static const int H = 13;
	static const int W = 6;
	int field[W][H];
	Field(){
		init();
	}
	void init(){
		for( int i=0; i<W; ++i )
			for( int j=0; j<H; ++j )
				field[i][j]=0;
	}
	int get( int x, int y ){
		if( x<1 || x>6 || y<1 || y>13 ) return -1;
		return field[x-1][y-1];
	}
	int set( int x, int y, int puyo ){
		if( x<1 || x>6 || y<1 || y>13 ) return -1;
		field[x-1][y-1] = puyo;
		return 0;
	}
	int set( int x, int puyo ){
		for( int y=1; y<=13; ++y ){
			if( get(x,y)==0 ){
				set(x,y,puyo);
				return 0;
			}
		}
		return -1;
	}
	int set( Action act, Tumo tumo ){
		if( act.id==0 ) return 0;
		int fx = act.pos;
		int sx = act.pos+(act.dir==1?1:(act.dir==3?-1:0));
		if( act.dir==2 ){
			set( sx, tumo.second );
			set( fx, tumo.first );
		} else {
			set( fx, tumo.first );
			set( sx, tumo.second );
		}
		return 0;
	}
private:
	int doCountConnection( int x, int y, int puyo, int flag[W][H] );
public:
	int countConnection( int x, int y ){
		const int puyo = get(x,y);
		if( puyo<1 || 4<puyo ) return 0;
		int flag[W][H] = {0};
		return doCountConnection( x, y, puyo, flag );
	}
	int deleteConnection( int x, int y ){
		printf("delete:(%d,%d)\n",x,y);
		return 0;
	}
};

struct Next{
	static const int SIZE = 128;
	Tumo next[SIZE];
	Next(){
		std::random_device rnd;
		for( int i=0; i<SIZE; ++i ){
			std::pair<int,int> tumo;
			tumo.first  = ((size_t)rnd())%4 + 1;
			tumo.second = ((size_t)rnd())%4 + 1;
			next[i] = tumo;
		}
	}
	Next( char array_byte[] ){
		for( int i=0; i<SIZE; ++i ){
			std::pair<int,int> tumo;
			tumo.first  = array_byte[i*2];
			tumo.second = array_byte[i*2+1];
			next[i] = tumo;
		}
	}
	Tumo get( int turn ){
		return next[turn%SIZE];
	}
};


struct Game{

	Next next;
	Field field[2];
	int turn;      // 経過ターン数
	vector<Action> history[2];
	Action action[2];
	int status[2];

	Game(){
		turn = 0;
	}
	Field &getMyField(){
		return field[0];
	};
	Field &getEnemyField(){
		return field[1];
	};
	void setNext( Next next ){
		this->next = next;
	}
	Tumo getNextTumo( int num, int player_id ){ // num手先のツモを返す
		int cnt = 0; // 現在までのパスでない手数を数える
		for( size_t i=0; i<history[player_id-1].size(); ++i ) cnt+=(history[player_id-1][i].id==1);
		return next.get(cnt+num);
	}
	void setAction( Action act, int player_id ){
		action[player_id-1] = act;
	}
	Action getAction( int player_id ){
		return action[player_id-1];
	}
	int tryForwardChain(int player_id){
		for( int x=1; x<=6; ++x ){
		for( int y=1; y<=12; ++y ){
			int n = field[player_id-1].countConnection(x,y);
			if( n>=4 ){
				field[player_id-1].deleteConnection(x,y);
			}
		}}
		return 0;
	}
	void goNextStep(){
		field[0].set(action[0],getNextTumo(0,1));
		field[1].set(action[1],getNextTumo(0,2));
		history[0].push_back(action[0]);
		history[1].push_back(action[1]);
		// 連鎖判定
		tryForwardChain(1);
		tryForwardChain(2);
		action[0] = Action();
		action[1] = Action();
		turn++;
	}
	int getStatus( int player_id ){
		return status[player_id-1];
	}
};
