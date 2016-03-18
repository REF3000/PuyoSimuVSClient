#include "PuyoAI.h"

class OldAI : public PuyoAI {

public:
	Action think( Field field, std::vector<Tumo> next, Field field2, std::vector<Tumo> next2 );
};
