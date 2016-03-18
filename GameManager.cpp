#include "GameManager.h"
#include "PuyoAIUtility.h"

#define axis_x(id) ( m_player[(id)].x )
#define axis_y(id) ( m_player[(id)].y )
#define sub_x(id)  ( m_player[(id)].x + ((m_player[(id)].dir==1)?1:(m_player[(id)].dir==3)?-1:0) )
#define sub_y(id)  ( m_player[(id)].y + ((m_player[(id)].dir==0)?1:(m_player[(id)].dir==2)?-1:0) )

GameManager::GameManager(){

}

void GameManager::init(){
	m_game.init();
	// 画像データ読み込み
	m_ui.load();

	// いろいろ初期化
	Player ini = { 3,12,0,-1,false };
	m_player[0] = ini;
	m_player[1] = ini;
}

void GameManager::start(){

}

void GameManager::update(){

}

void GameManager::draw(){
	
	m_ui.drawBackGround();
	m_ui.drawField( m_game.getMyField(),    1 );
	m_ui.drawField( m_game.getEnemyField(), 2 );
	m_ui.drawTumo( m_player[0].x, m_player[0].y, m_player[0].dir, m_game.getNextTumo(0,1), 1 );
	m_ui.drawTumo( m_player[1].x, m_player[1].y, m_player[1].dir, m_game.getNextTumo(0,2), 2 );
	m_ui.drawNext( m_game.getNextTumo(1,1), m_game.getNextTumo(2,1), 1 );
	m_ui.drawNext( m_game.getNextTumo(1,2), m_game.getNextTumo(2,2), 2 );
	m_ui.drawScore( m_game.getScore(1), 1 );
	m_ui.drawScore( m_game.getScore(2), 2 );
	m_ui.drawFrame();
	m_ui.drawOjamaNotice( m_game.getOjamaNotice(1) + m_game.getOjamaStock(1), 1 );
	m_ui.drawOjamaNotice( m_game.getOjamaNotice(2) + m_game.getOjamaStock(2), 2 );
	
	m_ui.drawDebugInfo( m_game );
}

void GameManager::fin(){
	m_ui.free();
}

bool GameManager::checkAvailablePosition( int id ){
	if( axis_y(id) >= 14 ) return false;
	if( m_game.getMyField().get(axis_x(id),axis_y(id))!=0 ) return false;
	if( m_game.getMyField().get(sub_x(id) ,sub_y(id) )!=0 ) return false;
	return true;
}

bool GameManager::canQuickTurn( int id ){
	int       x = m_player[id].x;
	int       y = m_player[id].y;
	Field field = m_game.getField( id+1 );
	if( (m_player[id].dir==0 || m_player[id].dir==2) &&
		field.get(x+1,y)!=0 && field.get(x-1,y) &&
		field.get(x,y-1)==0 && field.get(x,y+1)==0 ) return true;
	return false;
}
bool GameManager::processQuickTurn( int id ){
	if( canQuickTurn(id) ){
		if( m_player[id].quick_flag ){
			m_player[id].dir = (m_player[id].dir+2)%4;
			if( !checkAvailablePosition(id) ) ++m_player[id].y;
			m_player[id].quick_flag = false;
		} else {
			m_player[id].quick_flag = true;
		}
		return true;
	} else {
		m_player[id].quick_flag = false;
	}
	return false;
}

void GameManager::moveRight( int player_id ){
	++axis_x(player_id-1);
	if( checkAvailablePosition(player_id-1) ) return;
	--axis_x(player_id-1);
}

void GameManager::moveLeft( int player_id ){
	--axis_x(player_id-1);
	if( checkAvailablePosition(player_id-1) ) return;
	++axis_x(player_id-1);
}

void GameManager::turnRight( int player_id ){
	int id = player_id-1;
	// quickturn判定を最初に行う
	if( processQuickTurn(id) ) return;
	// とりあえず回転させてダメなら色々
	m_player[id].dir = (m_player[id].dir+1)%4;
	if( checkAvailablePosition(id) ) return;
	switch( m_player[id].dir ){
	case 1:
		--m_player[id].x;
		if( checkAvailablePosition(id) ) return;
		++m_player[id].x;
		break;
	case 3:
		++m_player[id].x;
		if( checkAvailablePosition(id) ) return;
		--m_player[id].x;
		break;
	case 2:
		++m_player[id].y;
		if( checkAvailablePosition(id) ) return;
		--m_player[id].y;
		break;
	}
	m_player[id].dir = (m_player[id].dir+3)%4;
}

void GameManager::turnLeft( int player_id ){
	int id = player_id-1;
	// quickturn判定を最初に行う
	if( processQuickTurn(id) ) return;
	// とりあえず回転させてダメなら色々
	m_player[id].dir = (m_player[id].dir+3)%4;
	if( checkAvailablePosition(id) ) return;
	switch( m_player[id].dir ){
	case 1:
		--m_player[id].x;
		if( checkAvailablePosition(id) ) return;
		++m_player[id].x;
		break;
	case 3:
		++m_player[id].x;
		if( checkAvailablePosition(id) ) return;
		--m_player[id].x;
		break;
	case 2:
		++m_player[id].y;
		if( checkAvailablePosition(id) ) return;
		--m_player[id].y;
		break;
	}
	m_player[id].dir = (m_player[id].dir+1)%4;
}

void GameManager::setAction( int player_id, int action_id ){
	m_player[player_id-1].action_id = action_id;
}
void GameManager::setAction( int player_id, Action action ){
	int id = player_id-1;
	m_player[id].action_id = action.id;
	m_player[id].x   = action.pos;
	m_player[id].dir = action.dir;
}
Action GameManager::getAction( int player_id ){
	int id = player_id-1;
	return Action( m_player[id].action_id, m_player[id].x, m_player[id].dir );
}
void GameManager::goNext(){
	m_game.setAction( Action(m_player[0].action_id,m_player[0].x,m_player[0].dir), 1 );
	m_game.setAction( Action(m_player[1].action_id,m_player[1].x,m_player[1].dir), 2 );
	m_game.goNextStep();
	for(int i=0;i<2;++i){
		m_player[i].x   = 3;
		m_player[i].y   = 12;
		m_player[i].dir = 0;
		m_player[i].action_id = -1;
		m_player[i].quick_flag = false;
	}
}

void GameManager::setNext( Next next ){
	m_game.setNext( next );
}

bool GameManager::isChain( int player_id ){
	return canFireField( m_game.getMyField() );
}