#pragma once
#include "Game.h"
#include "UI.h"



class GameManager {

	struct Player{
		int x;
		int y;
		int dir;
		int action_id;
	};

	Game m_game;
	UI   m_ui;
	Player m_player[2];

	bool checkAvailablePosition( int id );

public:

	GameManager();

	void init();
	void start();
	void update();
	void draw();
	void fin();

	void moveRight( int player_id );
	void moveLeft ( int player_id );
	void turnRight( int player_id );
	void turnLeft ( int player_id );
	void setAction( int player_id, int action_id );
	void goNext();
};