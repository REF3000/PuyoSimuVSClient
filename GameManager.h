#pragma once
#include "Game.h"
#include "AnnimationManager.h"


class GameManager {

	struct Player{
		int x;
		int y;
		int dir;
		int action_id;
		bool quick_flag;
	};

	Game m_game;
	Player m_player[2];
	AnnimationManager m_am;

	bool checkAvailablePosition( int id );
	bool canQuickTurn( int id );
	bool processQuickTurn( int id );
	bool doTurnRight( int player_id );
	bool doTurnLeft( int player_id );

public:

	GameManager();

	void init();
	void start();
	void update();
	void draw();
	void fin();

	void   moveRight( int player_id );
	void   moveLeft ( int player_id );
	void   turnRight( int player_id );
	void   turnLeft ( int player_id );
	void   setAction( int player_id, int action_id );
	void   setAction( int player_id, Action action );
	Action getAction( int player_id );
	void   setNext  ( Next next );
	void   setOjamaTable( char *buf );
	bool   isChain  ( int player_id );
	void   goNext();

	Game &getGame(){ return m_game; }
};
