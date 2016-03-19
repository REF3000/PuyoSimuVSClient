#include "UI.h"
#include "DxLib.h"

const int CELL_W = 30;
const int CELL_H = 30;
const int GAP  = 270;  // 1p‚Æ2p‚ÌŠÔŠu
const int P1_FIELD_LEFT = 50;
const int P1_FIELD_TOP  = 60;
const int P2_FIELD_LEFT = 410;
const int P2_FIELD_TOP  = 60;
#define DX(x) (CELL_W*(x))
#define DY(y) (CELL_H*(y))
POS getPosOnField( int x, int y, int player_id ){
	int sx = (player_id==1) ? 50 : 410;
	int sy = 60;
	return POS( sx+DX(x-1), sy+DY(13-y) );
}


void UI::load(){
	gh_back  = LoadGraph( "data/back.png" );
	gh_frame = LoadGraph( "data/frame.png" );
	LoadDivGraph( "data/ojama.png" , 7 , 7 , 1 , 24 , 24 , gh_ojama ) ;
	LoadDivGraph( "data/puyo.png" , 7 , 7 , 1 , 30 , 30 , gh_puyo ) ;
	sh_move   = LoadSoundMem( "data/move.wav" );
	sh_rotate = LoadSoundMem( "data/rotate.wav" );
	sh_notice = LoadSoundMem( "data/notice.wav" );
	sh_drop   = LoadSoundMem( "data/drop.wav" );
	sh_match  = LoadSoundMem( "data/match.wav" );
}
void UI::free(){
	DeleteGraph( gh_back );
	for( int i=0; i<7; ++i ) DeleteGraph( gh_ojama[i] );
	for( int i=0; i<7; ++i ) DeleteGraph( gh_puyo[i]  );
}

void UI::update(){
	am_drop.update();
}

void UI::drawPuyo( int x, int y, int puyo ){
	switch( puyo ){
	case 1: DrawGraph( x, y, gh_puyo[0], TRUE ); break;
	case 2: DrawGraph( x, y, gh_puyo[1], TRUE ); break;
	case 3: DrawGraph( x, y, gh_puyo[2], TRUE ); break;
	case 4: DrawGraph( x, y, gh_puyo[3], TRUE ); break;
	case 9: DrawGraph( x, y, gh_puyo[5], TRUE ); break;
	}
}

void UI::drawBackGround(){
    DrawGraph( 0, 0, gh_back, TRUE );
}
void UI::drawFrame(){
    DrawGraph( 0, 0, gh_frame, TRUE );
}

void UI::drawField( Field field, int player_id ){
	for(int x=1; x<=6; ++x ){
		for(int y=13; y>=1; --y ){
			POS pos = getPosOnField(x,y,player_id);
			drawPuyo( pos.x, pos.y, field.get(x,y) );
		}
	}
}

void UI::drawTumo( int x, int y, int dir, Tumo tumo, int player_id, bool highlight ){
	int fx  = x;
	int fy  = y;
	int sx  = x+(dir==1?1:(dir==3?-1:0));
	int sy  = y+(dir==0?1:(dir==2?-1:0));
	POS fp = getPosOnField(fx,fy,player_id);
	POS sp = getPosOnField(sx,sy,player_id);
	drawPuyo( fp.x, fp.y, tumo.first );
	drawPuyo( sp.x, sp.y, tumo.second );
}

void UI::drawNext( Tumo next1, Tumo next2, int player_id ){
	POS p1[] = { POS(260,120), POS(290,150) };
	POS p2[] = { POS(350,120), POS(320,150) };
	POS *p = (player_id==1) ? p1 : p2;
	drawPuyo( p[0].x, p[0].y,    next1.second );
	drawPuyo( p[0].x, p[0].y+30, next1.first );
	drawPuyo( p[1].x, p[1].y,    next2.second );
	drawPuyo( p[1].x, p[1].y+30, next2.first );
}

void UI::drawScore( int score, int player_id ){
	POS pos = (player_id==1) ? POS(260,400) : POS(290,430);
	DrawFormatString( pos.x, pos.y, GetColor(0,0,0), "%d", score );
}

void UI::drawOjamaNotice( int ojama_num, int player_id ){
	const int SX = (player_id==1) ? 50 : 410;
	const int SY = 20;

	vector<int> ojama;
	while( ojama_num>0 ){
		if( ojama_num >= 400 ){ ojama_num-=400; ojama.push_back(5); continue; }
		if( ojama_num >= 300 ){ ojama_num-=300; ojama.push_back(4); continue; }
		if( ojama_num >= 200 ){ ojama_num-=200; ojama.push_back(3); continue; }
		if( ojama_num >=  30 ){ ojama_num-= 30; ojama.push_back(2); continue; }
		if( ojama_num >=   6 ){ ojama_num-=  6; ojama.push_back(1); continue; }
		if( ojama_num >=   1 ){ ojama_num-=  1; ojama.push_back(0); continue; }
	}
	int lim=65;
	int x = SX;
	int y = SY;
	for( size_t i=0; i<ojama.size(); ++i ){
		int o = ojama[i];
		if( o==0 ) lim-= 5;
		else       lim-=10;
		if( lim<0 ) break; // 6.5’´‚¦‚½‚çÈ—ª
		DrawExtendGraph( x, y, x+36, y+36, gh_ojama[o], true );
		x += (o==0) ? 20:28;
	}
}





void UI::drawDebugInfo( Game game ){
	// 1p
	DrawFormatString( 270,10,GetColor(255,0,0),"%d:%d", game.getOjamaStock(1), game.getOjamaNotice(1) );
	// 2p
	DrawFormatString( 340,10,GetColor(255,0,0),"%d:%d", game.getOjamaStock(2), game.getOjamaNotice(2) );
}


void UI::playMove(){
	PlaySoundMem( sh_move, DX_PLAYTYPE_BACK );
}

void UI::playRotate(){
	PlaySoundMem( sh_rotate, DX_PLAYTYPE_BACK );
}

void UI::playNotice(){
	PlaySoundMem( sh_notice, DX_PLAYTYPE_BACK );
}

void UI::playDrop(){
	PlaySoundMem( sh_drop, DX_PLAYTYPE_BACK );
}

void UI::playMatch(){
	PlaySoundMem( sh_match, DX_PLAYTYPE_BACK );
}
