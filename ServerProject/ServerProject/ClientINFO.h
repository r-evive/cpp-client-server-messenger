#pragma once
#include <string>
#include <chrono>
#include <time.h>

using namespace std;

class ClientINFO {
protected:
	int id;
	string name;
	string connectionDate;
public:
	int GetID() { return this->id; }
	string GetName() { return this->name; }
	string GetConnectionDate() { return this->connectionDate; }
	void SetID(int value) { this->id = value; }
	void SetName(string value) { this->name = value; }
	void SetConnectionDate();
	string GetCurrentDate();
};