#pragma once
#include "Puyo.h"
#include <vector>

class PuyoAI {
public:
	virtual Action think( Field field, std::vector<Tumo> next, Field field2, std::vector<Tumo> next2 )=0;
	
};

class SampleAI : public PuyoAI {
public:
	Action think( Field field, std::vector<Tumo> next, Field field2, std::vector<Tumo> next2 ){
		return Action(1,2,1);
	}
};