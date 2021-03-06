#ifndef SYSCONSOLE_H
#define SYSCONSOLE_H

#include "sysmsgbus.h"
#include <iostream>
#include "sysipc.h"

using std::string;

class sysConsole : public Node
{
public:
	sysConsole() {};
	sysConsole(string s) :Node(s) {};
	virtual ~sysConsole() {};

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		string input;
		std::getline(std::cin, input);
		if (!input.empty())
		{
			message msg(sName, input);
			if (!msg.empty())
			{
				qOut->push(msg);
			}
		}
		
		return 0;
	}
};

class sysDetachedConsole : public sysConsole
{
public:
	sysDetachedConsole() {};
	sysDetachedConsole(string s) :sysConsole(s) {};
	virtual ~sysDetachedConsole() {};

	void startUp() { mmaper.startUp(); }
	void shutDown() { mmaper.shutDown(); }

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		int return_code = sysConsole::executeCommands(qIn, qOut);
		if(!qOut->empty())
		{
			mmaper.saveAndEraseQueue(*qOut);
		}
		return return_code;
	}

protected:
	mmapSaverFromChild mmaper;
};



class sysCout : public Node
{
public:
	sysCout() {};
	sysCout(string s) :Node(s) {};
	virtual ~sysCout() {};



	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		while (!qIn->empty())
		{
			message msg = qIn->getMsgAndPop();

			if (msg.value() == "silent") { bIsSilent = true; }
			if (msg.value() == string("verbose")) { bIsSilent = false; }

			if (!bIsSilent) std::cout << ">> " << msg.str() << "\n";
			
		}
		return 0;
	}
private:
	bool bIsSilent=false;
};


class sysWriter : public Node
{
public:
	sysWriter() {};
	sysWriter(string s) :Node(s) {};
	virtual ~sysWriter() {};

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		while (!qIn->empty())
		{
			message msg = qIn->getMsgAndPop();
			std::cout << ">> " << msg.str() << "\n";
		}
		return 0;
	}
};

#endif
