#include "timercontrol.h"

ServerSplitter::Timer::Timer()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    this->ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Error at socket()");
    }

    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(16834); // default livesplit port
    result = inet_pton(AF_INET, "127.0.0.1", &clientService.sin_addr);
    if (result <= 0) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("inet_pton failed or invalid IP address");
    }

    if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to connect to the server.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }

    // send test message
    std::string testcommand = "reset\n";
    int sendResult = send(ConnectSocket, testcommand.c_str(), (int)testcommand.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to send message.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }
}
ServerSplitter::Timer::~Timer()
{
    closesocket(ConnectSocket);
    WSACleanup();
}
void ServerSplitter::Timer::sendCommand(const std::string& command) {
    std::string fullCommand = command + "\n";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
    this->ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Error at socket()");
    }
    if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to connect to the server.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }

    int sendResult = send(ConnectSocket, fullCommand.c_str(), (int)fullCommand.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send message.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }
}
void ServerSplitter::Timer::start()
{
    Timer::sendCommand("start");
}
void ServerSplitter::Timer::stop()
{
    Timer::sendCommand("stop");
}
void ServerSplitter::Timer::pause()
{
    Timer::sendCommand("pause");
}
void ServerSplitter::Timer::resume()
{
    Timer::sendCommand("resume");
}
void ServerSplitter::Timer::split()
{
    Timer::sendCommand("split");
}
void ServerSplitter::Timer::reset()
{
    Timer::sendCommand("reset");
}
//use skip() instead of split() if timer is paused at the skip call
void ServerSplitter::Timer::skip()
{
    Timer::sendCommand("skipsplit");
}

bool ServerSplitter::sendCommand(const std::string& command) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in clientService;
    int result;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return false;
    }

    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket()." << std::endl;
        WSACleanup();
        return false;
    }

    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(16834); // default livesplit port
    result = inet_pton(AF_INET, "127.0.0.1", &clientService.sin_addr);
    if (result <= 0) {
        std::cerr << "inet_pton failed or invalid IP address." << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }

    // connecting to the server
    if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect." << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }

    // send message
    int sendResult = send(ConnectSocket, command.c_str(), (int)command.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Send failed." << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }

    closesocket(ConnectSocket);
    WSACleanup();
    return true;
}

void ServerSplitter::pauseTimer() {
    sendCommand("pause");
}

void ServerSplitter::resumeTimer() {
    sendCommand("resume");
}

void ServerSplitter::startTimer() {
    sendCommand("start");
}

void ServerSplitter::split() {
    sendCommand("split");
}

void ServerSplitter::resetTimer() {
    sendCommand("reset");
}

ServerSplitter::Timer ServerSplitter::createTimer(bool debug)
{
    smartptr<Timer> timer(nullptr);
    while (true) {
        try {
            timer = new Timer();
        }
        catch (std::runtime_error ex) {
            if (debug) {
                std::cout << std::endl << ex.what();
            }
            Sleep(2000);
            continue;
        }
        break;
    }
    return *timer;
}

template<class T>
ServerSplitter::smartptr<T>::smartptr(T* ptr)
{
    this->ptr = ptr;
}

template<class T>
ServerSplitter::smartptr<T>::~smartptr()
{
    delete ptr;
}

template<class T>
T ServerSplitter::smartptr<T>::operator*()
{
    return *ptr;
}

template<class T>
void ServerSplitter::smartptr<T>::operator=(T* ptr)
{
    delete this->ptr;
    this->ptr = ptr;
}
