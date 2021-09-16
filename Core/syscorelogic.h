#ifndef SYSCORELOGIC_H
#define SYSCORELOGIC_H

#include "sysmsgbus.h"
#include <iostream>

class sysCoreLogic : public Node
{
public:
	sysCoreLogic() {};
	sysCoreLogic(string s) :Node(s) {};
	virtual ~sysCoreLogic() {};

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		while (!qIn->empty())
		{
			message msg = qIn->getMsgAndPop();

			if (msg.value() == "quit") { sEngineState->bEngineInitialized = false; continue; }
			if (msg.value() == "pause") { sEngineState->bPaused = true; continue; }
			if (msg.value() == "continue") { sEngineState->bPaused = false; continue; }

		}
		return 0;
	}
};





#endif
