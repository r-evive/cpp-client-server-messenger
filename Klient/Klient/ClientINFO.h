#pragma once
#include <string>

using namespace std;

class ClientINFO
{
private:
	int id;
	string name;
public:
	string GetName() { return this->name; }
	int GetID() { return this->id; }
	void SetID(int value) { this->id = value; }
	void SetName(string name) { this->name = name; }
	string GetInitData();
};


