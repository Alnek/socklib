#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <deque>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#define LOG(expression) \
{\
    std::stringstream out;\
    out << expression;\
    Console::GetInstance().Print(out.str());\
}

typedef void (*ConsoleCallback)(const std::vector<std::string>&, void*);

class Console
{
public:
    static Console& GetInstance();

    void Process();
    void Print(const std::string& message);
    void Print(const char* message, size_t len = 0);

    void BindAction(const char* command, ConsoleCallback action, void* context = nullptr);
private:
    struct Action
    {
        ConsoleCallback func;
        void* context;
    };

    Console();

    void InitCmd();
    void CleanLine();
    void Execute();
    void PrevCommand();
    void NextCommand();

    std::string mCommand;
    std::map<std::string, Action> mActions;

    std::deque<std::string> mHistory;
    uint32_t mHistoryIndex;
};

#endif
