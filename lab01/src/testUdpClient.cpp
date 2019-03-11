#include <iostream>
#include <winsock2.h>

using namespace std;

int main(){
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);

    string toSend = "Hello udp";
    const char* sendbuf = toSend.c_str();

    SOCKADDR_IN server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);

    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

    sendto(out, sendbuf, sizeof(sendbuf) + 1, 0, (SOCKADDR *) &server, sizeof(server));
    closesocket(out);
    WSACleanup();
}
