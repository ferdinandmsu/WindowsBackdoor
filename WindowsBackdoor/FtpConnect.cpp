#include "FtpConnect.h"

FtpConnect::FtpConnect(const std::string& host, const std::string& username, const std::string& password)
	: m_host(host), m_username(username), m_password(password)
{
}

FtpConnect::~FtpConnect()
{
}

void FtpConnect::connect()
{
	FtpIo = InternetOpenA("SystemConnection", INTERNET_OPEN_TYPE_DIRECT, m_host.c_str(), 0, INTERNET_FLAG_CACHE_IF_NET_FAIL);
	FtpSession = InternetConnectA(FtpIo, m_host.c_str(), INTERNET_DEFAULT_FTP_PORT, m_username.c_str(), m_password.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
}

bool FtpConnect::upload_file(const std::string& local_path, const std::string& server_name)
{
	BOOL result = FtpPutFileA(FtpSession, local_path.c_str(), server_name.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
	return result;
}

bool FtpConnect::retr_file(const std::string& local_path, const std::string& server_name)
{
	BOOL result = FtpGetFileA(FtpSession, server_name.c_str(), local_path.c_str(), 0, 0, FTP_TRANSFER_TYPE_BINARY, INTERNET_FLAG_HYPERLINK);
	return result;
}

bool FtpConnect::mkdir(const std::string& folder_name)
{
	BOOL result = FtpCreateDirectoryA(FtpSession, folder_name.c_str());
	return result;
}

bool FtpConnect::cd(const std::string& server_path)
{
	BOOL result = FtpSetCurrentDirectoryA(FtpSession, server_path.c_str());
	return result;
}