#pragma once
#include "includes.h"
#include "externs.h"

extern bool demoui_loaded;

// very dirty
class ccommand {
public:
    static char* getLastCommand();
    static void start();
    static void ccommand_register(std::string str, std::function<void()> func);
    static void ccommand_hook(std::string str, std::function<void()> func);
    static void ccommand_hook_only(std::string str, std::function<void()> func);
private:
    static std::vector<ccommand> commands;
    std::string name;
    std::function<void()> func;
    bool checkonly = false;
    ccommand(std::string& name, std::function<void()> func, bool checkonly = false);
    void command_trigger();
};

#define CCOMMAND_REGISTER(name) \
    [&]() { \
        ccommand::ccommand_register(name, [&]() 

#define CCOMMAND_HOOK(name) \
    [&]() { \
        ccommand::ccommand_hook(name, [&]() 

// checks if registered text exists at the start of the command, for example if registered text = "hello" and entered console command = "hello world", it will work. But! if entered command = "world hello" or "he" it wont work.
#define CCOMMAND_HOOK_ONLY(name) \
    [&]() { \
        ccommand::ccommand_hook_only(name, [&]() 

#define REGISTER_EXECUTE() \
        ); \
    }()

#define CCOMMAND_ALIAS(name, command) engine->ConsoleCommand("alias " name " " command)

#define CCOMMAND CCOMMAND_REGISTER
#define REGISTER REGISTER_EXECUTE
#define CCOMMAND_LIST void ccommand_list
#define REGISTER_COMMANDS() ccommand_list()
#define ALIAS CCOMMAND_ALIAS