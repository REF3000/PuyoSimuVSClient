#include "Game.h"


int Field::doCountConnection( int x, int y, int puyo, int flag[W][H] ){
	if( !( 1<=x && x<=6 && 1<=y && y<=12 ) ) return 0;
	if( flag[x-1][y-1] ) return 0;
	if( get(x,y)!=puyo ) return 0;
	return doCountConnection( x, y+1, puyo, flag ) +
		   doCountConnection( x+1, y, puyo, flag ) +
		   doCountConnection( x, y-1, puyo, flag ) +
		   doCountConnection( x-1, y, puyo, flag ) + 1;
}