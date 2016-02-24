#include "Tokenizer.h"

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <fstream>
#endif

void Tokenizer::Insert(string line, string del)
{
	currLine = line;
	delim = del;
} 

string Tokenizer::NextToken()
{
	int index = currLine.find(delim);
	if (index != MAX_FIND_VALUE)
	{
		string response = currLine.substr(0, index);
		currLine = currLine.substr(index + 1, MAX_FIND_VALUE);
		return response;
	}
	else
	{
		string response = currLine;
		currLine == "";
		return response;
	}
}
#ifdef DEBUG
int main()
{ 
	Tokenizer t;
	t.Insert("Pizza is good yeah", ",");
	string result = t.NextToken();
	if (result == "")
	{
		cout << result << endl;
		cin >> result;
	}

	t.Insert("cat, mat, hat", ",");
	result = t.NextToken();
	cout << result << endl;
	result = t.NextToken();
	cout << result << endl;
	result = t.NextToken();
	cout << result << endl;
	cin >> result;

}
#endif