#include <iostream>
#include <winsock2.h>
#include <thread>

using namespace std;

int main() {
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);

    SOCKADDR_IN serverHint;
    serverHint.sin_addr.s_addr = INADDR_ANY;
    serverHint.sin_family = AF_INET;
    serverHint.sin_port = htons(9000);

    SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
    bind(in, (SOCKADDR *) &serverHint, sizeof(serverHint));

    SOCKADDR_IN client;
    int clientLength = sizeof(client);

    char recvbuf[1024];
    memset(recvbuf, 0, sizeof recvbuf);
    ZeroMemory(&client, clientLength);

    this_thread::sleep_for(10s);

    recvfrom(in, recvbuf, 1024, 0, (SOCKADDR *) &client, &clientLength);

    cout << "Received msg: " << recvbuf << endl;

    closesocket(in);
    WSACleanup();
}