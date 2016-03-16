#pragma once
#include "Puyo.h"
#include <vector>

class PuyoAI {
public:
	virtual Action think( Field field, std::vector<Tumo> next )=0;
	
};

class SampleAI : public PuyoAI {
public:
	Action think( Field field, std::vector<Tumo> next ){
		return Action(1,2,1);
	}
};