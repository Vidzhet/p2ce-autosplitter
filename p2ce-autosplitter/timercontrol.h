#pragma once
#include <winsock2.h>
#include <ws2tcpip.h> // Äëÿ inet_pton

#pragma comment(lib, "ws2_32.lib")
#include "includes.h"
#define CATCH_CONSOLE_DEBUG true

//library for send commands to livesplt
namespace ServerSplitter {
    template<class T>
    class smartptr {
    private:
        T* ptr;
    public:
        smartptr(T* ptr);
        ~smartptr();
        T operator *();
        void operator =(T* ptr);
    };

    class Timer {
    private:
        WSADATA wsaData;
        SOCKET ConnectSocket = INVALID_SOCKET;
        struct sockaddr_in clientService;
        int result;
    public:
        Timer();
        ~Timer();
        void sendCommand(const std::string& command);
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
        void setcomparison(std::string comparison);
        void setgametime(const std::string& time);
        std::string gettime();
    };

    bool sendCommand(const std::string& command);
    void pauseTimer();
    void resumeTimer();
    void startTimer();
    void split();
    void resetTimer();
    Timer createTimer(bool debug = false);
}