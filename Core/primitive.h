#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <string>

class tBox
{
public:
	tBox() {};
	~tBox() {};

private:
	std::string primitive_name = "box";
	float vertices[24] = { 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 
		                   0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1 };
};


#endif