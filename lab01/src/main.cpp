#include <iostream>
#include <winsock2.h>
#include <thread>
#include <chrono>
#include <sstream>

using namespace std;

struct client_args {
    int i;
    string clientID;
    u_short listenPort;
    u_short nextPort;
    bool hasToken;
    int selectedProtocol; // TCP - 1, UDP - 2
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void client_routine(client_args args) {
    string ID = args.clientID;
    auto listenOn = htons(args.listenPort);
    auto sendTo = htons(args.nextPort);

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);

    SOCKET socketSendTo;
    SOCKADDR_IN addrSendTo;

    SOCKET server;
    SOCKADDR_IN addrServer;

    SOCKET socketReceiveFrom;
    SOCKADDR_IN addrReceiveFrom;

    if (args.i % 2 == 0) {
        socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = sendTo;

        connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

        server = socket(AF_INET, SOCK_STREAM, 0);
        addrServer.sin_addr.s_addr = INADDR_ANY;
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = listenOn;

        bind(server, (SOCKADDR *) &addrServer, sizeof(addrServer));
        listen(server, 0);

        int clientAddrSize = sizeof(addrReceiveFrom);
        socketReceiveFrom = accept(server, (SOCKADDR *) &addrReceiveFrom, &clientAddrSize);
    } else {
        server = socket(AF_INET, SOCK_STREAM, 0);
        addrServer.sin_addr.s_addr = INADDR_ANY;
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = listenOn;

        bind(server, (SOCKADDR *) &addrServer, sizeof(addrServer));
        listen(server, 0);

        int clientAddrSize = sizeof(addrReceiveFrom);
        socketReceiveFrom = accept(server, (SOCKADDR *) &addrReceiveFrom, &clientAddrSize);

        socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = sendTo;

        connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
    }

    if (args.hasToken) {
        string toSend = ID + "#Hello from " + ID;
        char buf1[1024];
        strcpy(buf1, toSend.c_str());
        send(socketSendTo, buf1, sizeof(buf1), 0);
    }

    while(true){
        char buf2[1024];
        recv(socketReceiveFrom, buf2, sizeof(buf2), 0);

        char* id = strtok(buf2, "#");
        char* msg = strtok(NULL, "#");

        cout << ID << " received msg for " << id << "; the message is: " << msg << endl;
        memset(buf2, 0, sizeof(buf2));

        this_thread::sleep_for(chrono::milliseconds(1000));

        string toSend = ID + "#Hello from " + ID;
        char buf1[1024];
        strcpy(buf1, toSend.c_str());
        send(socketSendTo, buf1, sizeof(buf1), 0);
    }

    closesocket(socketSendTo);
    closesocket(socketReceiveFrom);
}
#pragma clang diagnostic pop

int main(int argc, char **argv) {
    int numberOfClients = atoi(argv[1]);
    if (numberOfClients < 2) {
        cout << "Wrong number of clients" << endl;
        return 1;
    }

    int protocol = atoi(argv[2]);
    if (protocol != 1 && protocol != 2) {
        cout << "Wrong protocol" << endl;
        return 1;
    }

    auto clients = new thread[numberOfClients];

    for (int i = 0; i < numberOfClients; i++) {

        client_args clientArgs;

        stringstream ss;
        ss << "client" << (i + 1);
        clientArgs.clientID = ss.str();

        clientArgs.i = i;
        clientArgs.listenPort = 9000 + (i % numberOfClients);
        clientArgs.nextPort = 9000 + ((i + 1) % numberOfClients);
        clientArgs.hasToken = i == 0;
        clientArgs.selectedProtocol = protocol;

        clients[i] = thread(client_routine, clientArgs);
    }

    for (int i = 0; i < numberOfClients; i++) {
        clients[i].join();
    }

    delete[] clients;
    WSACleanup();
    return 0;
}