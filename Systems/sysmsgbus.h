#ifndef SYSMSGBUS_H
#define SYSMSGBUS_H

#include <queue>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>
#include <thread>
#include "engineState.h"
#include <list>


using std::queue;
using std::vector;
using std::string;
using std::mutex;
using std::lock_guard;
using std::thread;
using std::stringstream;




/**
Classes for exchange of messages between systems. Node subscribes clients - other nodes.
After that notify calls processMsg function for all messag in queue for each subscribed client.
Later will be changed to multitopic bus and multithreading.
*/
#define EXIT_CODE -1

class message
{
public:
	message() {};
	message(string _sProducerName, string _sValue) : sProducerName(_sProducerName), sValue(_sValue) 
	{ set_empty(); };
	message(string sDumpedMsg) 
	{
		size_t pos = sDumpedMsg.find(delimiter());
		sProducerName = sDumpedMsg.substr(0, pos);
		
		sDumpedMsg.erase(0, pos + delimiter().length());
		sValue = sDumpedMsg;

		set_empty();
	};
	~message() {};

	bool empty() { return bEmpty; }
	void set_empty() { bEmpty = (sValue == ""); }

	string producer() { return sProducerName; }
	string value() { return sValue; }
	string delimiter() { return sDelimiter; }
	
	string str(){ return producer() + delimiter() + value(); }
	
private:
	string sProducerName;
	string sValue;
	string sDelimiter = " -> ";
	bool bEmpty = true;
};

class messageQueue
{
public:
	messageQueue() {};
	messageQueue(string& sDumpedQueue)
	{
		load(sDumpedQueue);
		/*
		string input;
		stringstream ss;
		ss << sDumpedQueue;

		int num_cmd = 0;
		std::getline(ss, input);
		stringstream tmp;
		tmp << input;
		tmp >> num_cmd;

		messageQueue out;
		for (int i = 0; i < num_cmd; i++)
		{
			std::getline(ss, input);
			message cmd(input);
			q.push(cmd);
		}*/
	};
	virtual ~messageQueue() {};

	void load(string& sDumpedQueue)
	{
		string input;
		stringstream ss;
		ss << sDumpedQueue;

		int num_cmd = 0;
		std::getline(ss, input);
		stringstream tmp;
		tmp << input;
		tmp >> num_cmd;

		messageQueue out;
		for (int i = 0; i < num_cmd; i++)
		{
			std::getline(ss, input);
			message cmd(input);
			//q.push(cmd);
			q.push_back(cmd);
		}
	}
	
	//message front(){ return q.front(); }
	//void pop() { q.pop(); }
	//void push(message mVal) { q.push(mVal); }
	//bool empty(){return q.empty();}

	message front() { return q.front(); }
	void pop() { q.pop_front(); }
	void push(message mVal) { q.push_back(mVal); }
	void erase() { q.erase(q.begin(), q.end()); }
	bool empty() { return q.empty(); }

	message getMsgAndPop()
	{
		message x = front();
		pop();
		return x;
	}

	string str()
	{
		//queue<message> q_copied = q;
		std::list<message>q_copied = q;
		size_t size = q_copied.size();

		stringstream ss;
		ss << size;
		string out;
		ss >> out;
		out += "\n";

		while (!q_copied.empty())
			{
				message cmd = q_copied.front();
				//q_copied.pop();
				q_copied.pop_back();
				out += cmd.str();
				out += "\n";
			}
		return out;		
		
	}

	static messageQueue fromString(string& s)
	{
		string input;
		stringstream ss;
		ss << s;
		int num_cmd = 0;
		std::getline(ss, input);
		stringstream tmp;
		tmp << input;
		tmp >> num_cmd;
		messageQueue out;
		for (int i = 0; i < num_cmd; i++)
		{			
			std::getline(ss, input);
			message cmd(input);
			out.push(cmd);
		}
		return out;
	}

	void join(messageQueue& qAdd)
	{
		q.splice(q.end(), *(qAdd.getContainerPtr()));
	}
	
	std::list<message>* getContainerPtr()
	{
		return &q;
	}

private:
	//queue<message> q;
	std::list<message>q;
};


class Node
{
public:
	Node(string _s = "sysDummy"): sName(_s) {};
	virtual ~Node() {};

	string getName() { return sName; }
	void subscribeClient(Node& c) { vClients.push_back(&c); }	
	bool bInputEmpty() { return qInputMsgQueue.empty(); }
	bool bOutputEmpty() { return qOutputMsgQueue.empty(); }

	

	void connectEngineState(engineState* _sState)
	{ 
		sEngineState = _sState;
		for (auto c : vClients)
		{
			c->connectEngineState(sEngineState);
		}
	}

	void startUp(){}
	void shutDown()
	{
		for (auto it = vClients.rbegin(); it != vClients.rend(); it++)
		{
			(*it)->shutDown();
		}

	}


	message getNextInputMsg()
	{
		lock_guard<mutex> lck(mtx);
		return qInputMsgQueue.getMsgAndPop();
	}

	message getNextOutputMsgThreadsave()
	{
		lock_guard<mutex> lck(mtx);
		return qOutputMsgQueue.getMsgAndPop();
	}


	void runDetached() 
	{
		thread t(&Node::continuousRun, this);
		t.detach();
	}



	int callSystemThreadsave()
	{
		collectResults();
		messageQueue qInp;
		copyInpQueueAndClean(&qInp);
		messageQueue qOut;
		int out = executeCommands(&qInp, &qOut);
		updateOutputQueue(qOut);


		return out;
	}

	void collectResults()
	{
		for (auto c : vClients)
		{
			while (!(c->bOutputEmpty()))
			{
				enqueueMsg(c->getNextOutputMsgThreadsave());
			}
		}
	}

	void callClients()
	{
		for (auto c : vClients)
		{
			c->callSystemThreadsave();
			//std::cout << c->getName() << '\n';
		}
	}

	void copyInpQueueAndClean(messageQueue* q)
	{
		lock_guard<mutex> lck(mtx);
		*q = qInputMsgQueue;
		while (!bInputEmpty())
		{
			qInputMsgQueue.pop();
		}
	}

	void updateOutputQueue(messageQueue& q)
	{
		lock_guard<mutex> lck(mtx);		
		while (!q.empty())
		{
			qOutputMsgQueue.push(q.getMsgAndPop());
		}
	}


	void continuousRun()
	{
		while (sEngineState->bEngineInitialized)
		{
			if (!bPausable || !sEngineState->bPaused)
			{
				int result = callSystemThreadsave();
			}
			
		}
	}

	void enqueueMsg(message msg)
	{
		lock_guard<mutex> lck(mtx);
		if (!msg.empty())
		{
			qInputMsgQueue.push(msg);
		}
	}
	virtual int executeCommands(messageQueue* qIn, messageQueue* qOut ) = 0;

	int clientSize()
	{
		return static_cast<int>(vClients.size());
	}
	
	

protected:
	messageQueue qInputMsgQueue;
	messageQueue qOutputMsgQueue;
	vector<Node*> vClients;
	string sName = "sysDummy";
	mutex mtx;
	engineState* sEngineState;
	bool bPausable = false;
};

class sysBus: public Node
{
public:
	sysBus(){};
	sysBus(string s) :Node(s) {};
	virtual ~sysBus() {};

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		while (!qIn->empty())
		{
			message msg = qIn->getMsgAndPop();
			for (auto c : vClients)
			{
				c->enqueueMsg(msg);
			}
		}
		return 0;
	}
	
private:
	messageQueue busExecuteQueue;
};


#endif