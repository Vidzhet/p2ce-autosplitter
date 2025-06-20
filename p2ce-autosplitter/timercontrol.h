#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#include "includes.h"
//#include "panorama.h"
#define CATCH_CONSOLE_DEBUG true
#define TIMER_NOINIT true

//library for sending commands to livesplt using stupid ass tcp server
namespace ServerSplitter {
    class Timer {
    private:
        WSADATA wsaData;
        SOCKET ConnectSocket = INVALID_SOCKET;
        struct sockaddr_in clientService;
        int result;
        std::string tryGetResponse(std::string command);
    public:
        Timer(bool noinit = false);
        ~Timer();
        void sendCommand(std::string command);
        std::string getResponse(std::string command);
        void start();
        void stop();
        void pause();
        void resume();
        void split();
        void reset();
        void skip();
        void unsplit();
        void pauseGametime();
        void resumeGametime();
        void setcomparison(const std::string& comparison);
        void setgametime(const std::string& time);
        bool timerPaused();
        std::string getGametime();
        std::string gettime();
        int getSplitIndex();
        std::string ping();
    };

    bool sendCommand(const std::string& command);
    void pauseTimer();
    void resumeTimer();
    void startTimer();
    void split();
    void resetTimer();
    Timer createTimer();
}