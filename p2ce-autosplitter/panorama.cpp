#include "panorama.h"
#include "externs.h"

bool livesplit_connected = false;


void timer_update_connection(ServerSplitter::Timer& timer) {
    while (true) {
        try {
            timer.sendCommand("ping");
            // if successful;
            livesplit_connected = true;
        }
        catch (const std::runtime_error&) {
            livesplit_connected = false;
            timer = ServerSplitter::createTimer(); // recreating timer
        }
        Sleep(2000);
    }
}
