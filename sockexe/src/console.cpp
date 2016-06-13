#include "console.h"

#include <iostream>
#include <string>
#include <windows.h>

extern bool runFlag;

namespace
{
    const char WELCOME[] = "> ";
}

Console& Console::GetInstance()
{
    static Console sInstance;
    return sInstance;
}

Console::Console()
{
    mCommand.reserve(256);
    InitCmd();
}

void Console::InitCmd()
{
    std::cout << WELCOME << mCommand;
}

void Console::CleanLine()
{
    const size_t cnt = sizeof(WELCOME) + mCommand.length();
    for (size_t i = 0; i != cnt; ++i)
    {
        std::cout << '\b' << ' ' << '\b';
    }
}

void Console::Execute()
{
    if ("" == mCommand) { InitCmd(); }
    //else if ("" == mCommand) {}
    else if ("exit" == mCommand) { runFlag = false; }
    else
    {
        std::cout << "unknown command: " << mCommand << std::endl;
        mCommand.clear();
        InitCmd();
    }

    mCommand.clear();
}

void Console::Process()
{
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

    INPUT_RECORD records[128];
    DWORD numRead = 0;
    BOOL res = ReadConsoleInputA(h, records, 128, &numRead);
    if (TRUE != res) return;
    for (int i = 0; i != numRead; ++i)
    {
        INPUT_RECORD& record = records[i];
        if (KEY_EVENT == record.EventType && 1 == record.Event.KeyEvent.bKeyDown)
        {
            const char c = record.Event.KeyEvent.uChar.AsciiChar;
            if ('\b' == c)
            {
                if (false == mCommand.empty())
                {
                    std::cout << '\b' << ' ' << '\b';
                    mCommand.pop_back();
                }
            }
            else if ('\r' == c)
            {
                std::cout << std::endl;
                Execute();
            }
            else
            {
                std::cout << c;
                mCommand.push_back(c);
            }
        }
    }
}

void Console::Print(const std::string& message)
{
    CleanLine();
    std::cout << message << std::endl;
    InitCmd();
}

void Console::Print(const char* message, size_t len)
{
    if (nullptr == message) return;
    if (0 == len) len = strlen(message);

    Print(std::string(message, len));
}
