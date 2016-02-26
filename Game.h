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
		if( x>=1 && x<=6 && y==14 ) return 0;
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
				return y;
			}
		}
		return -1;
	}
	int set( Action act, Tumo tumo ){
		if( act.id==0 ) return 0;
		int fx = act.pos;
		int sx = act.pos+(act.dir==1?1:(act.dir==3?-1:0));
		int y;
		if( act.dir==2 ){
			    set( sx, tumo.second );
			y = set( fx, tumo.first );
		} else {
			y = set( fx, tumo.first );
			    set( sx, tumo.second );
		}
		return 13-y;
	}
private:
	int doCountConnection( int x, int y, int puyo, int flag[W][H] );
	int doDeleteConnection( int x, int y, int puyo );
public:
	int countConnection( int x, int y ){
		const int puyo = get(x,y);
		if( !(1<=puyo && puyo<=4) ) return 0;
		int flag[W][H] = {0};
		return doCountConnection( x, y, puyo, flag );
	}
	int deleteConnection( int x, int y ){
		const int puyo = get(x,y);
		if( !(1<=puyo && puyo<=4) ) return 0;
		return doDeleteConnection( x, y, puyo );
	}
	int fall(){
		int fallen_flag = 0;
		for( int x=1; x<=6; ++x ){
			vector<int> tmp;
			for( int y=1; y<=13; ++y ) if(get(x,y)) tmp.push_back(get(x,y));
			for( int y=1; y<=13; ++y ) set(x,y,0), fallen_flag=1;
			for( size_t i=0; i<tmp.size(); ++i) set( x,i+1,tmp[i] );
		}
		return fallen_flag;
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
	int status[2]; // 0:非連鎖時 1:連鎖中
	int score[2];
	int ojama_score[2];
	int chain_count[2];
	int ojama_notice[2];
	int ojama_stock[2];
	int ojama_count[2];
	static const int OJAMA_TABLE[6];

	Game(){
		init();
	}
	void init(){
		turn = 0;
		for( int i=0; i<2; ++i ){
			status[i] = 0;
			score[i] = 0;
			action[i] = Action();
			chain_count[i] = 0;
			ojama_notice[i] = 0;
			ojama_stock[i] = 0;
			ojama_count[i] = 0;
		}
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
	void addScore( int id, int score ){
		this->score[id] += score;
		ojama_score[id] += score;
	}
	void processOjamaScore( int id ){
		int ojama = ojama_score[id]/70;
		ojama_score[id] -= ojama*70;
		// 相殺（notice）
		if( ojama<ojama_notice[id] ){
			ojama_notice[id] -= ojama;
			return;
		}
		ojama -= ojama_notice[id];
		ojama_notice[id] = 0;
		// 相殺（stock）
		if( ojama<ojama_stock[id] ){
			ojama_stock[id] -= ojama;
			return;
		}
		ojama -= ojama_stock[id];
		ojama_stock[id] = 0;
		// 敵おじゃま予告増加
		ojama_stock[(id+1)%2] += ojama;
	}
	void forwardChain( int id ){
		++chain_count[id];
		const int A[] = {0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512};
		const int B[] = {0,3,6,12,24};
		const int C[] = {0,2,3,4,5,6,7,10};
		int score = 0;
		int bonas = A[chain_count[id]-1];
		int color_flag[5] = {};
		for( int x=1; x<=6; ++x ){
			for( int y=1; y<=12; ++y ){
				int n = field[id].countConnection(x,y);
				if( n>=4 ){
					score += n*10;
					color_flag[field[id].get(x,y)] = 1;
					field[id].deleteConnection(x,y);
					bonas += C[(n<11)?(n-4):10];
				}
			}
		}
		int color_cnt=0;
		for(int i=1; i<=4; ++i ) color_cnt+=color_flag[i];
		bonas += B[color_cnt-1];
		if( bonas==0 ) bonas=1;
		addScore( id, score*bonas );
		processOjamaScore( id );
	}
	bool canFire( int id ){
		for( int x=1; x<=6; ++x ){
			for( int y=1; y<=12; ++y ){
				int n = field[id].countConnection(x,y);
				if( n>=4 ) return true;
			}}
		return false;
	}
	void fallOjama( int id ){
		if( ojama_notice[id]<=0 ) return;
		int num = (ojama_notice[id]>30) ? 30 : ojama_notice[id];
		for( int i=0; i<num; ++i ){
			const int P = ojama_count[id]++%6;
			field[id].set(OJAMA_TABLE[P],9); // TODO:OJAMAは定数
		}
		ojama_notice[id] -= num;
	}
	void transmitOjama( int id ){
		if( ojama_stock[id]<=0 ) return;
		ojama_notice[id] += ojama_stock[id];
		ojama_stock[id] = 0;
	}
	void goNextStep(){
		for( int i=0; i<2; ++i ){
			if( action[i].id==-1 ){
				printf("actionがセットされていない :player%d",i+1);
				return;
			}
		}
		int ojama_trans_flag[2]={};
		for( int id=0; id<2; ++id ){
			if( status[id]==0 ){ // 非連鎖中
				int fall_d = field[id].set(action[id],getNextTumo(0,id+1));
				addScore( id, fall_d );
				if( canFire(id) ){
					status[id] = 1;
					continue;
				}
				// TODO:窒息判定
				fallOjama( id );
				// TODO:窒息判定2
			} else {             // 連鎖中
				forwardChain(id);
				field[id].fall();
				if( canFire(id) ){
					continue;
				}
				// TODO:窒息判定
				fallOjama( id );
				ojama_trans_flag[ (id+1)%2 ] = true;
				// TODO:窒息判定2
				status[id] = 0;
				chain_count[id] = 0;
			}
			history[id].push_back(action[id]);
			action[id] = Action();
		}
		for( int id=0; id<2; ++id ){
			if( ojama_trans_flag[id] ) transmitOjama( id );
		}
		turn++;
	}
	int getStatus( int player_id ){
		return status[player_id-1];
	}
	int getOjamaStock( int player_id ){
		return ojama_stock[player_id-1];
	}
	int getOjamaNotice( int player_id ){
		return ojama_notice[player_id-1];
	}
	int getScore( int player_id ){
		return score[player_id-1];
	}
};
