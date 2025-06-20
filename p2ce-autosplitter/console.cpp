#include "console.h"
#include "commands.h"

char* ccommandBuf;
std::vector<ccommand> ccommand::commands;

ccommand::ccommand(std::string& name, std::function<void()> func, bool checkonly) {
    this->name = name;
    this->func = func;
    this->checkonly = checkonly;
}

void ccommand::ccommand_register(std::string str, std::function<void()> func)
{
    engine->ClientCmd_Unrestricted(std::string("alias " + str).c_str()); // dirty command register
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
    if (this->name == ccommandBuf || checkonly && this->name == std::string(ccommandBuf, this->name.size())) { // if last entered command equals to registered command
        this->func();
        engine->ClientCmd_Unrestricted("ccommand_executed"); // changing ccommandBuf to execute func only single time
    }
}

char* ccommand::getLastCommand()
{
    return ccommandBuf;
}

void ccommand_setup() {
    engine->ClientCmd_Unrestricted("alias ccommand_executed"); // define ccommand in game
    ccommandBuf = memory::find_const_str("engine.dll", "alias ccommand_executed"); // getting pointer to cbuf in engine.dll by finding it by its value
    engine->ClientCmd_Unrestricted("ccommand_executed"); // define ccommand in game
    while (ccommandBuf == nullptr || std::string(ccommandBuf) != "ccommand_executed") {
        engine->ClientCmd_Unrestricted("ccommand_executed");
        ccommandBuf = memory::find_str("engine.dll", "ccommand_executed");
    }
    //MessageBoxA(nullptr, "ccommand initialized", "a", MB_OK);
    std::thread(ccommand::start).detach(); // ccommand loop thread
}

void ccommand::start()
{
    REGISTER_COMMANDS();
    std::cout << ccommandBuf << '\t' << std::hex << reinterpret_cast<uintptr_t>(ccommandBuf) << std::endl;
    while (true) {
        for (ccommand& cmd : ccommand::commands) {
            cmd.command_trigger();
            Sleep(10);
        }
    }
}
