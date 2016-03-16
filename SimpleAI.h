#include "PuyoAI.h"

class SimpleAI : public PuyoAI {
public:
	Action think( Field field, std::vector<Tumo> next );
};
