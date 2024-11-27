#include "timercontrol.h"
#include "panorama.h"

ServerSplitter::Timer::Timer(bool noinit)
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
    if (!noinit) {
        if (connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            WSACleanup();
            throw std::runtime_error("Failed to connect to the server.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
        }
    
        // send test message
        std::string testcommand = "switchto gametime\n\r";
        int sendResult = send(ConnectSocket, testcommand.c_str(), (int)testcommand.length(), 0);
        if (sendResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            WSACleanup();
            throw std::runtime_error("Failed to send message.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
        }
    }
}
ServerSplitter::Timer::~Timer()
{
    closesocket(ConnectSocket);
    WSACleanup();
}
void ServerSplitter::Timer::sendCommand(const std::string& command) {
    std::string fullCommand = command + "\r\n";

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
    while (sendResult == SOCKET_ERROR) {
        //console_log("send err. Connection restored"); // debug instead of throwing exeption
        sendResult = send(ConnectSocket, fullCommand.c_str(), (int)fullCommand.length(), 0); // attept to fix problem below
        //throw std::runtime_error("Failed to send message.\tWSAGetLastError: " + std::to_string(WSAGetLastError())); // throws error sometimes
        Sleep(100);
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
void ServerSplitter::Timer::unsplit()
{
    Timer::sendCommand("unsplit");
}
void ServerSplitter::Timer::pauseGametime()
{
    Timer::sendCommand("pausegametime");
}
void ServerSplitter::Timer::resumeGametime()
{
    Timer::sendCommand("unpausegametime");
}
void ServerSplitter::Timer::setcomparison(std::string comparison)
{
    std::string command = "setcomparison " + comparison;
    Timer::sendCommand(command);
}
void ServerSplitter::Timer::setgametime(const std::string& time) {
    std::string command = "setgametime " + time;
    sendCommand(command);
}
std::string ServerSplitter::Timer::gettime()
{
    std::string command = "getcurrenttime\r\n";

    if (send(ConnectSocket, command.c_str(), (int)command.length(), 0) == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to send gettime command.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }

    char buffer[1024];
    int recvResult = recv(ConnectSocket, buffer, sizeof(buffer), 0);
    if (recvResult > 0) {
        buffer[recvResult] = '\0'; // Ensure the buffer is null-terminated
        return std::string(buffer);
    }
    else if (recvResult == 0) {
        throw std::runtime_error("Connection closed by server.");
    }
    else {
        throw std::runtime_error("recv failed.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }
}

std::string ServerSplitter::Timer::ping()
{
    std::string command = "ping\r\n";

    if (send(ConnectSocket, command.c_str(), (int)command.length(), 0) == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to ping LiveSplit.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }

    char buffer[1024];
    int recvResult = recv(ConnectSocket, buffer, sizeof(buffer), 0);
    if (recvResult > 0) {
        buffer[recvResult] = '\0'; // Ensure the buffer is null-terminated
        return std::string(buffer);
    }
    else if (recvResult == 0) {
        throw std::runtime_error("Connection closed by server.");
    }
    else {
        throw std::runtime_error("recv failed.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }
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
    std::shared_ptr<Timer> timer(nullptr);
    while (true) {
        try {
            timer = std::make_shared<Timer>();
        }
        catch (std::runtime_error& ex) {
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