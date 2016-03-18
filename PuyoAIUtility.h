#pragma once
#include "Puyo.h"

struct FireInfo{
	int limit;
	int score;
};

FireInfo fireField( Field &field );