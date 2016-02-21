#include "Game.h"


int Field::doCountConnection( int x, int y, int puyo, int flag[W][H] ){
	if( !( 1<=x && x<=6 && 1<=y && y<=12 ) ) return 0;
	if( flag[x-1][y-1] ) return 0;
	if( get(x,y)!=puyo ) return 0;
	flag[x-1][y-1] = 1;
	return doCountConnection( x, y+1, puyo, flag ) +
		   doCountConnection( x+1, y, puyo, flag ) +
		   doCountConnection( x, y-1, puyo, flag ) +
		   doCountConnection( x-1, y, puyo, flag ) + 1;
}
int Field::doDeleteConnection( int x, int y, int puyo ){
	if( !( 1<=x && x<=6 && 1<=y && y<=12 ) ) return 0;
	if( get(x,y)==9 ){ set(x,y,0); return 0; } // ‚¨‚¶‚á‚Üíœ
	if( get(x,y)!=puyo ) return 0;
	set(x,y,0);
	return doDeleteConnection( x, y+1, puyo ) +
		   doDeleteConnection( x+1, y, puyo ) +
		   doDeleteConnection( x, y-1, puyo ) +
		   doDeleteConnection( x-1, y, puyo ) + 1;
}
const int Game::OJAMA_TABLE[] = {1,2,3,4,5,6};