#pragma once
#include "Game.h"
#include "AnnimationManager.h"

struct POS{
	int x,y;
	POS(int x=0,int y=0){
		this->x = x;
		this->y = y;
	}
};









class UI{
	int gh_back;
	int gh_frame;
	int gh_ojama[10];
	int gh_puyo[10];
	AnnimationManager am_drop;
	int sh_move;
	int sh_rotate;
	int sh_notice;
	int sh_drop;
	int sh_match;
	int sh_win;
	int sh_lose;
	int sh_draw;

	void drawPuyo( int x, int y, int puyo );

public:
	void load();
	void free();

	void update();

	void drawBackGround();
	void drawFrame();
	void drawField( Field field, int player_id );
	void drawTumo( int x, int y, int dir, Tumo tumo, int player_id, bool highlight=false );
	void drawNext( Tumo next1, Tumo next2, int player_id );
	void drawScore( int score, int player_id );
	void drawOjamaNotice( int ojama_num, int player_id );
	void playMove();
	void playRotate();
	void playNotice();
	void playDrop();
	void playMatch();
	void playWin();
	void playLose();
	void playDraw();

	void drawDebugInfo( Game game );

	static UI& getInstance(){
		static UI inst;
		return inst;
	}
	static UI& gi(){ // ‰¡’…
		return UI::getInstance();
	}
};