#pragma once

#include <iostream>
#include <Windows.h>
#include <WinInet.h>
#include <string>

#include "WinWrapper.h"

#pragma comment(lib, "WinInet.lib")

class FtpConnect
{

public:

	FtpConnect(const std::string& host, const std::string& username, const std::string& password);
	~FtpConnect();

	void connect();

	bool upload_file(const std::string& local_path, const std::string& server_name);

	bool retr_file(const std::string& local_path, const std::string& server_name);

	bool mkdir(const std::string& folder_name);

	bool cd(const std::string& server_path);

private:
	std::string m_host;
	std::string m_username;
	std::string m_password;

	HINTERNET FtpIo, FtpSession;

};