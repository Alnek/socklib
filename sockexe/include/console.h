#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <string>
#include <sstream>

#define LOG(expression) \
{\
    std::stringstream out;\
    out << expression;\
    Console::GetInstance().Print(out.str());\
}

class Console
{
public:
    static Console& GetInstance();

    void Process();
    void Print(const std::string& message);
    void Print(const char* message, size_t len = 0);

private:
    Console();

    void InitCmd();
    void CleanLine();
    void Execute();

    std::string mCommand;
};

#endif
