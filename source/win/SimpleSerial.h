#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <chrono>
#include <thread>
#include <time.h>
#include <fstream>

//using namespace std;
//Thou shalt not using namespace in a header file >:(

class SimpleSerial
{

private:
	HANDLE io_handler_;
	COMSTAT status_;
	DWORD errors_;

	std::string syntax_name_;
	char front_delimiter_;
	char end_delimiter_;

	void CustomSyntax(std::string syntax_type);

public:
	SimpleSerial(const char* com_port, DWORD COM_BAUD_RATE);

	std::string ReadSerialPort(int reply_wait_time, std::string syntax_type);
	bool WriteSerialPort(const char* data_sent);
	bool CloseSerialPort();
	~SimpleSerial();
	bool connected_;
};

