#pragma once

#include <string>

using namespace std;
// Jacob this class is awesome!!!
class Tokenizer
{
public:
	void Insert(string line, string del);

	string NextToken();
private:
	string currLine;
	string delim;
	const int MAX_FIND_VALUE = 4294967295;
};

