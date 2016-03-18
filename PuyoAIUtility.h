#pragma once
#include "Puyo.h"

bool canFireField( Field &field );

struct FireInfo{
	int limit;
	int score;
};

FireInfo fireField( Field &field );