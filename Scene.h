#pragma once
typedef int PARAM;

class Scene{
public:

	virtual void init( PARAM param )  = 0;
	virtual int update() = 0;
	virtual void draw()  = 0;
	virtual void fin()   = 0;

};

