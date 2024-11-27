#include "console.h"
#include "commands.h"

uintptr_t ccommandBuf = NULL;
std::vector<ccommand> ccommand::commands;

ccommand::ccommand(std::string& name, std::function<void()> func, bool checkonly) {
    this->name = name;
    this->func = func;
    this->checkonly = checkonly;
}

void ccommand::ccommand_register(std::string str, std::function<void()> func)
{
    engine->ConsoleCommand(std::string("alias " + str).c_str()); // dirty command register
    ccommand::commands.push_back(ccommand(str, func));
}

void ccommand::ccommand_hook(std::string str, std::function<void()> func)
{
    ccommand::commands.push_back(ccommand(str, func));
}
// checks if registered text exists at the start of the command, for example if registered text = "hello" and entered console command = "hello world", it will work. But! if entered command = "world hello" or "he" it wont work.
void ccommand::ccommand_hook_only(std::string str, std::function<void()> func)
{
    ccommand::commands.push_back(ccommand(str, func, true)); // true = hook_only is set
}

void ccommand::command_trigger()
{
    if (this->name == reinterpret_cast<char*>(ccommandBuf) || checkonly && this->name == std::string(reinterpret_cast<char*>(ccommandBuf), this->name.size())) { // if last entered command equals to registered command
        this->func();
        engine->ConsoleCommand("ccommand_executed"); // changing ccommandBuf to execute func only single time
    }
}

char* ccommand::getLastCommand()
{
    return reinterpret_cast<char*>(ccommandBuf);
}

void ccommand::start()
{
    ccommandBuf = engineBaseAddress + 0x1B96800;
    REGISTER_COMMANDS();
    engine->ConsoleCommand("alias ccommand_executed"); // define ccommand in game
    while (true) {
        for (ccommand& cmd : ccommand::commands) {
            cmd.command_trigger();
            Sleep(10);
        }
    }
}
