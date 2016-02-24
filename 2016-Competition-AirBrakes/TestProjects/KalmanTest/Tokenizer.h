#pragma once

#include <string>

using namespace std;

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

