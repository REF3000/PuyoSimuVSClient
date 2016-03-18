#include "PuyoAI.h"

class SimpleAI : public PuyoAI {
public:
	Action think( Field field, std::vector<Tumo> next, Field field2, std::vector<Tumo> next2 );
};
