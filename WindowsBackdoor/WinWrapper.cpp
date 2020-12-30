#include "WinWrapper.h"

std::string WinWrapper::read_file(const std::string &file_path)
{
	std::string text;
	std::string line;
	std::ifstream file(file_path);

	if (!file.is_open())
		throw ErrorCodes::FILE_ERROR;

	while (std::getline(file, line)) {
		text += line + "\n";
	}
	
	file.close();

	return text;
}

void WinWrapper::write_file(const std::string &file_path, const std::string &content)
{
	std::ofstream file(file_path);

	if (!file.is_open())
		throw ErrorCodes::FILE_ERROR;

	file << content;

	file.close();
}


bool WinWrapper::path_exists(const std::string& path)
{
	if (std::filesystem::exists(path))
		return true;
	return false;
}


std::string WinWrapper::get_username()
{
	char env[] = "USERNAME";
	DWORD username_len = 257;
	char buffer[4096];

	unsigned int out_size = GetEnvironmentVariableA(env, buffer, username_len);

	return std::string(buffer, out_size);
}


std::string WinWrapper::last_error_as_string()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

std::vector<std::string> WinWrapper::listdir(const std::string& path)
{
	std::vector<std::string> directory_content;

	for (const auto& val : std::filesystem::directory_iterator(path)) {
		std::string content = val.path().u8string();
		std::size_t last_idx = content.find_last_of("\\");
		directory_content.push_back(content.substr(last_idx + 1));
	}

	return directory_content;
}

std::string WinWrapper::random_string()
{
	std::string tmp_s;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	srand((unsigned)time(NULL) * 38);

	tmp_s.reserve(20);

	for (int i = 0; i < 20; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


	return tmp_s;
}

std::string WinWrapper::get_date()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");
	auto str = oss.str();

	return str;
}

BITMAPINFOHEADER WinWrapper::createBitmapHeader(int width, int height)
{
	BITMAPINFOHEADER  bi;

	// create a bitmap
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	return bi;
}

HBITMAP WinWrapper::GdiPlusScreenCapture(HWND hWnd)
{
	// get handles to a device context (DC)
	HDC hwindowDC = GetDC(hWnd);
	HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	// define scale, height and width
	int scale = 1;
	int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// create a bitmap
	HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	BITMAPINFOHEADER bi = WinWrapper::createBitmapHeader(width, height);

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that call HeapAlloc using a handle to the process's default heap.
	// Therefore, GlobalAlloc and LocalAlloc have greater overhead than HeapAlloc.
	DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char* lpbitmap = (char*)GlobalLock(hDIB);

	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);   //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// avoid memory leak
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hWnd, hwindowDC);

	return hbwindow;
}

bool WinWrapper::saveToMemory(HBITMAP* hbitmap, std::vector<BYTE>& data, std::string dataFormat)
{
	Gdiplus::Bitmap bmp(*hbitmap, nullptr);
	// write to IStream
	IStream* istream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &istream);

	// define encoding
	CLSID clsid;
	if (dataFormat.compare("bmp") == 0) { CLSIDFromString(L"{557cf400-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
	else if (dataFormat.compare("jpg") == 0) { CLSIDFromString(L"{557cf401-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
	else if (dataFormat.compare("gif") == 0) { CLSIDFromString(L"{557cf402-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
	else if (dataFormat.compare("tif") == 0) { CLSIDFromString(L"{557cf405-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
	else if (dataFormat.compare("png") == 0) { CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid); }

	Gdiplus::Status status = bmp.Save(istream, &clsid, NULL);
	if (status != Gdiplus::Status::Ok)
		return false;

	// get memory handle associated with istream
	HGLOBAL hg = NULL;
	GetHGlobalFromStream(istream, &hg);

	// copy IStream to buffer
	int bufsize = GlobalSize(hg);
	data.resize(bufsize);

	// lock & unlock memory
	LPVOID pimage = GlobalLock(hg);
	memcpy(&data[0], pimage, bufsize);
	GlobalUnlock(hg);
	istream->Release();
	return true;
}

std::string WinWrapper::screenshot(const std::string& path)
{
	std::string full_path = (std::filesystem::path(path) / std::filesystem::path((WinWrapper::get_date() + ".jpg"))).u8string();

	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// get the bitmap handle to the bitmap screenshot
	HWND hWnd = GetDesktopWindow();
	HBITMAP hBmp = WinWrapper::GdiPlusScreenCapture(hWnd);

	// save as png to memory
	std::vector<BYTE> data;
	std::string dataFormat = "jpg";

	if (WinWrapper::saveToMemory(&hBmp, data, dataFormat))
	{
		// save from memory to file
		std::ofstream fout(full_path, std::ios::binary);
		fout.write((char*)data.data(), data.size());
	}
	else
		return "";

	GdiplusShutdown(gdiplusToken);

	return full_path;
}


void WinWrapper::append_file(const std::string& file_path, const std::string& content)
{
	std::ofstream file(file_path, std::ios_base::app);

	if (!file.is_open())
		throw ErrorCodes::FILE_ERROR;

	file << content;

	file.close();
}