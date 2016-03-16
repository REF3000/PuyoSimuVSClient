#pragma once
#include <utility>
#include <random>
#include <vector>


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
			std::vector<int> tmp;
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
