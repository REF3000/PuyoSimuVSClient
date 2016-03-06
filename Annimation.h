#pragma once
#include "DxLib.h"
#include <iostream>
using namespace std;

class Annimation{
public:

	virtual int update();
	virtual void draw();

};

class sampleAnnimation : public Annimation{
public :
	int flame;
	int x,y;
	sampleAnnimation( int x, int y ){
		flame = 0;
		this->x = x;
		this->y = y;
	}

	int update(){
		if( ++flame >300 ) return -1;
		return 0;
	}

	void draw(){
		DrawFormatString( x,y,GetColor(255,255,255),"%d", flame );
	}

};

#if 0
class fallAnnimation : public Annimation{
public :
	int x,y;
	int color;
	int dist;
	fallAnnimation( int x, int y, int dist, int color ){
		this->x    = x;
		this->y    = y;
		this->dist = dist;
		this->color = color;
	}

	int update(){
		y += 10;
		if( y>dist ) return -1;
		return 0;
	}

	void draw(){
		DrawOval( x, y, 29/2, 29/2, color, true );
		//DrawFormatString( x,y,GetColor(255,255,255),"%d", y );
	}

};
#endif