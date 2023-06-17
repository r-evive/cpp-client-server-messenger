#pragma once
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

class Logs
{
private:
	string logsFilePath;
	ofstream* file;
public:
	Logs(string logsFilePath);
	void AppendToFile(string message);
	string GetCurrentTimeHeader();
	~Logs();
};

