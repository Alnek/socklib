#include "console.h"

#include "lockobject.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

namespace
{
    const char WELCOME[] = "> ";
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

Console& Console::GetInstance()
{
    static Console sInstance;
    return sInstance;
}

Console::Console()
{
    mLockObject = new LockObject;
    mCommand.reserve(256);
    InitCmd();
}

Console::~Console()
{
    delete mLockObject;
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
    if (true == mCommand.empty())
    {
        InitCmd();
        return;
    }

    std::vector<std::string> tokens;
    split(mCommand, ' ', tokens);

    std::string initialCommand;
    initialCommand.swap(mCommand);

    if (tokens.size() > 0)
    {
        mHistory.push_front(initialCommand);
        mHistoryIndex = 0;

        const std::string& command = tokens[0];
        for (auto it = mActions.begin(); it != mActions.end(); ++it)
        {
            if (it->first == command)
            {
                Action& action = it->second;
                (*action.func)(tokens, action.context);
                CleanLine();
                InitCmd();
                return;
            }
        }
    }

    LOG("unknown command: " << initialCommand);
}

void Console::PrevCommand()
{
    CleanLine();

    if (mHistory.size() > mHistoryIndex)
    {
        mCommand = mHistory[mHistoryIndex++];
    }

    InitCmd();
}

void Console::NextCommand()
{
    CleanLine();

    if (mHistoryIndex > 0) mHistoryIndex--;

    if (mHistoryIndex > 0 && mHistory.size() > mHistoryIndex)
    {
        mCommand = mHistory[mHistoryIndex - 1];
    }
    else
    {
        mCommand = "";
    }

    InitCmd();
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
            mLockObject->Lock();
            mCommandQueue.push(record);
            mLockObject->Unlock();
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

void Console::BindAction(const char* command, ConsoleCallback action, void* context)
{
    Action tmp;
    tmp.func = action;
    tmp.context = context;
    mActions[command] = tmp;
}

void Console::ProcessQueue()
{
    mLockObject->Lock();

    while(false == mCommandQueue.empty())
    {
        INPUT_RECORD& record = mCommandQueue.front();
        if (VK_RETURN == record.Event.KeyEvent.wVirtualKeyCode)
        {
            std::cout << std::endl;
            Execute();
        }
        else if (VK_BACK == record.Event.KeyEvent.wVirtualKeyCode)
        {
            if (false == mCommand.empty())
            {
                std::cout << '\b' << ' ' << '\b';
                mCommand.pop_back();
            }
        }
        else if (VK_UP == record.Event.KeyEvent.wVirtualKeyCode)
        {
            PrevCommand();
        }
        else if (VK_DOWN == record.Event.KeyEvent.wVirtualKeyCode)
        {
            NextCommand();
        }

        const char c = record.Event.KeyEvent.uChar.AsciiChar;
        if (0 == c || '\r' == c || '\b' == c)
        {
        }
        else
        {
            std::cout << c;
            mCommand.push_back(c);
        }

        mCommandQueue.pop();
    }

    mLockObject->Unlock();
}
