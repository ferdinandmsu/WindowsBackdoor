#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <gdiplus.h>
#include <gdiplusbitmap.h>
#include <gdiplusbase.h>

using namespace Gdiplus;

#pragma comment (lib,"gdiplus.lib")

enum class ErrorCodes
{
	MKDIR_ERROR = 0,
	FILE_ERROR = 1,
	INTERNAL_ERROR = 2,
	FTP_CONNECT_ERROR = 3
};

class WinWrapper
{
public:
	WinWrapper() = default;
	~WinWrapper() = default;

	static std::string read_file(const std::string& file_path);
	static void write_file(const std::string& file_path, const std::string& content);

	static bool path_exists(const std::string& path);
	static std::string get_username();

	static std::string last_error_as_string();
	static std::vector<std::string> listdir(const std::string& path);

	static std::string screenshot(const std::string& path);
	static std::string random_string();

	static std::string get_date();
	static BITMAPINFOHEADER createBitmapHeader(int width, int height);
	
	static HBITMAP GdiPlusScreenCapture(HWND hWnd);
	static bool saveToMemory(HBITMAP* hbitmap, std::vector<BYTE>& data, std::string dataFormat = "png");

	static void append_file(const std::string& file_path, const std::string& content);
};