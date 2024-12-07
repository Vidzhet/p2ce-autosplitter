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
void ServerSplitter::Timer::sendCommand(std::string command) {
    command += "\r\n";

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

    int sendResult = send(ConnectSocket, command.c_str(), (int)command.length(), 0);
    while (sendResult == SOCKET_ERROR) {
        //console_log("send err. Connection restored"); // debug instead of throwing exeption
        sendResult = send(ConnectSocket, command.c_str(), (int)command.length(), 0); // attept to fix problem below
        //throw std::runtime_error("Failed to send message.\tWSAGetLastError: " + std::to_string(WSAGetLastError())); // throws error sometimes
        Sleep(10);
    }
}
std::string ServerSplitter::Timer::tryGetResponse(std::string command)
{
    command += "\r\n";

    bool dirty = false;
    while (send(ConnectSocket, command.c_str(), static_cast<int>(command.length()), 0) == SOCKET_ERROR) {
        if (dirty) {
            std::thread MessageBoxThread(MessageBoxA, nullptr, "SOCKET ERROR ON send()!", "dirty", MB_OK);
            MessageBoxThread.detach();
            dirty = false;
        }
        std::cout << "SOCKET ERROR ON send()!\n";
        /*closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to send gettime command.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));*/
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(ConnectSocket, &readSet);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000; // 10 milliseconds

    int selectResult = select(0, &readSet, NULL, NULL, &timeout);
    if (selectResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("select failed.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }
    else if (selectResult == 0) {
        throw std::runtime_error("Timeout waiting for server response.");
    }

    // set socket to not blocking mode
    u_long mode = 1;
    if (ioctlsocket(ConnectSocket, FIONBIO, &mode) == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::runtime_error("Failed to set non-blocking mode.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }

    char buffer[128];
    int recvResult = recv(ConnectSocket, buffer, sizeof(buffer) - 1, 0);

    // restore socket to blocking mode
    mode = 0;
    ioctlsocket(ConnectSocket, FIONBIO, &mode);

    if (recvResult > 0) {
        buffer[recvResult] = '\0';
        return std::string(buffer);
    }
    else if (recvResult == 0) {
        throw std::runtime_error("Connection closed by server.");
    }
    else if (WSAGetLastError() == WSAEWOULDBLOCK) {
        throw std::runtime_error("Timeout: Data was not received within 0.01 seconds.");
    }
    else {
        throw std::runtime_error("recv failed.\tWSAGetLastError: " + std::to_string(WSAGetLastError()));
    }
}
// this will take over than 10 miliseconds!
std::string ServerSplitter::Timer::getResponse(std::string command) {
    while (true) {
        try {
            return this->tryGetResponse(command);
        }
        catch (const std::runtime_error& ex) {
            std::cout << ex.what() << std::endl;
        }
    }
}
void ServerSplitter::Timer::start()
{
    this->sendCommand("start");
}
void ServerSplitter::Timer::stop()
{
    this->sendCommand("stop");
}
void ServerSplitter::Timer::pause()
{
    this->sendCommand("pause");
}
void ServerSplitter::Timer::resume()
{
    this->sendCommand("resume");
}
void ServerSplitter::Timer::split()
{
    this->sendCommand("split");
}
void ServerSplitter::Timer::reset()
{
    this->sendCommand("reset");
}
//use skip() instead of split() if timer is paused at the skip call
void ServerSplitter::Timer::skip()
{
    this->sendCommand("skipsplit");
}
void ServerSplitter::Timer::unsplit()
{
    this->sendCommand("unsplit");
}
void ServerSplitter::Timer::pauseGametime()
{
    this->sendCommand("pausegametime");
}
void ServerSplitter::Timer::resumeGametime()
{
    this->sendCommand("unpausegametime");
}
void ServerSplitter::Timer::setcomparison(const std::string& comparison)
{
    this->sendCommand("setcomparison " + comparison);
}
void ServerSplitter::Timer::setgametime(const std::string& time) {;
    this->sendCommand("setgametime " + time);
}
bool ServerSplitter::Timer::timerPaused() { // fuck this shit
    //this->sendCommand("switchto gametime");
    std::cout << "timer paused?\n";
    auto temp = this->getGametime();
    std::cout << "\nfirst time: " << temp << std::endl;
    Sleep(1);
    std::cout << "\nlast time: " << this->getGametime() << std::endl; // bug on getGametime()!!!! (seems to be fixed)
    if (temp == this->getGametime()) { return true; }
    else { return false; }
}
std::string ServerSplitter::Timer::getGametime()
{
    return this->getResponse("getcurrentgametime");
}
std::string ServerSplitter::Timer::gettime()
{
    return this->getResponse("getcurrenttime");
}
int ServerSplitter::Timer::getSplitIndex()
{
    while (true) {
        try {
            return std::stoi(this->getResponse("getsplitindex")); // convert std::string to int (really buggy sometimes);
        }
        catch (std::exception&) {}
    }
}
std::string ServerSplitter::Timer::ping() { // THIS SHIT CAN SOMETIMES KILL GAME LOL
    return this->getResponse("ping");
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
        catch (const std::runtime_error& ex) {
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