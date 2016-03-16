#include "PuyoAI.h"

class OldAI : public PuyoAI {
public:
	Action think( Field field, std::vector<Tumo> next );
};
