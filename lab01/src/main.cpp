#include <iostream>
#include <winsock2.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>

#define MULTICAST_PORT 5000

using namespace std;

struct client_args {
    int i;
    string clientID;
    string nextClientID;
    string lastClientID;
    u_short backdoorPort;
    u_short listenPort;
    u_short nextPort;
    bool hasToken;
    bool newClient;
};

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT) {
        WSACleanup();
        throw 1;
    }
    return TRUE;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void client_routine_udp(client_args args) {
    string ID = args.clientID;
    auto listenOn = args.listenPort;
    auto sendTo = args.nextPort;

    SOCKET socketSendTo;
    SOCKADDR_IN addrSendTo;

    SOCKET socketReceiveFrom;
    SOCKADDR_IN addrReceiveFrom;

    SOCKET socketBackdoor;
    SOCKADDR_IN addrBackdoor;

    SOCKET socketLoggers;
    SOCKADDR_IN addrLoggers;
    if (!args.newClient) {

        if (args.i == 0) {
            // configure backdoor socket
            addrBackdoor.sin_addr.s_addr = INADDR_ANY;
            addrBackdoor.sin_family = AF_INET;
            addrBackdoor.sin_port = args.backdoorPort;

            socketBackdoor = socket(AF_INET, SOCK_DGRAM, 0);
            u_long iMode = 1;
            ioctlsocket(socketBackdoor, FIONBIO, &iMode);
            bind(socketBackdoor, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));
        }

        addrReceiveFrom.sin_addr.s_addr = INADDR_ANY;
        addrReceiveFrom.sin_family = AF_INET;
        addrReceiveFrom.sin_port = listenOn;

        socketReceiveFrom = socket(AF_INET, SOCK_DGRAM, 0);
        bind(socketReceiveFrom, (SOCKADDR *) &addrReceiveFrom, sizeof(addrReceiveFrom));

        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = sendTo;
        socketSendTo = socket(AF_INET, SOCK_DGRAM, 0);

        this_thread::sleep_for(chrono::milliseconds(500));

        if (args.hasToken) {
            string toSend = args.nextClientID + "#0#Hello from " + ID;
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
        }
    } else {
        // new client
        addrBackdoor.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrBackdoor.sin_family = AF_INET;
        addrBackdoor.sin_port = sendTo;
        socketBackdoor = socket(AF_INET, SOCK_DGRAM, 0);

        stringstream ss;
        ss << ID << "#" << args.listenPort;
        string toSend = ss.str();
        char buf1[1024];
        strcpy(buf1, toSend.c_str());
        sendto(socketBackdoor, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));

        char buf2[1024];
        SOCKADDR_IN tmp;
        int tmpLength = sizeof(tmp);
        recvfrom(socketBackdoor, buf2, sizeof(buf2), 0, (SOCKADDR *) &tmp, &tmpLength);

        char *id = strtok(buf2, "#");
        char *msg = strtok(nullptr, "#");

        args.nextClientID = id;
        closesocket(socketBackdoor);


        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = stoi(msg);
        socketSendTo = socket(AF_INET, SOCK_DGRAM, 0);

        addrReceiveFrom.sin_addr.s_addr = INADDR_ANY;
        addrReceiveFrom.sin_family = AF_INET;
        addrReceiveFrom.sin_port = listenOn;
        socketReceiveFrom = socket(AF_INET, SOCK_DGRAM, 0);
        bind(socketReceiveFrom, (SOCKADDR *) &addrReceiveFrom, sizeof(addrReceiveFrom));

        cout << "New client successfully added." << endl;
    }

    addrLoggers.sin_addr.s_addr = inet_addr("224.0.0.3");
    addrLoggers.sin_family = AF_INET;
    addrLoggers.sin_port = htons(MULTICAST_PORT);
    socketLoggers = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketLoggers, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));
    setsockopt(socketLoggers, IPPROTO_IP, IP_MULTICAST_TTL, nullptr, 0);

    while (true) {
        char buf2[1024];
        SOCKADDR_IN tmp;
        int tmpLength = sizeof(tmp);
        recvfrom(socketReceiveFrom, buf2, sizeof(buf2), 0, (SOCKADDR *) &tmp, &tmpLength);

        char *id = strtok(buf2, "#");
        string type = strtok(nullptr, "#");
        char *msg = strtok(nullptr, "#");

        if (ID == id) {
            stringstream sstream;
            sstream << ID << " received msg: " << msg << endl;
            char buf7[1024];
            strcpy(buf7, sstream.str().c_str());
            sendto(socketLoggers, buf7, sizeof(buf7), 0, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));

            this_thread::sleep_for(chrono::milliseconds(1000));

            if (type == "0") {
                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
            } else {
                string newNextID = strtok(msg, "$");
                string newNextPortString = strtok(nullptr, "$");
                args.nextClientID = newNextID;
                args.nextPort = stoi(newNextPortString);
                addrSendTo.sin_port = args.nextPort;

                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
            }

        } else {
            stringstream ss;
            ss << id << "#" << type << "#" << msg;
            string toSend = ss.str();
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
        }

        if (args.i == 0) {
            SOCKADDR_IN addrNewClient;
            int lengthAddrNewClient = sizeof(addrNewClient);
            char buf3[1024];
            int readBytes = recvfrom(socketBackdoor, buf3, sizeof(buf3), 0, (SOCKADDR *) &addrNewClient,
                                     &lengthAddrNewClient);
            if (readBytes != SOCKET_ERROR) {
                // there is a new client
                char *newClientID = strtok(buf3, "#");
                char *newClientListenPort = strtok(nullptr, "#");

                string toSend1 = args.lastClientID + "#1#" + newClientID + "$" + newClientListenPort;
                char buf1[1024];
                strcpy(buf1, toSend1.c_str());
                sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

                args.lastClientID = newClientID;

                stringstream ss;
                ss << ID << "#" << args.listenPort;
                string toSend2 = ss.str();
                char buf2[1024];
                strcpy(buf2, toSend2.c_str());
                sendto(socketBackdoor, buf2, sizeof(buf2), 0, (SOCKADDR *) &addrNewClient, lengthAddrNewClient);

                char discard[1024];
                SOCKADDR_IN discardTmp;
                int discardTmpLength = sizeof(discardTmp);
                recvfrom(socketReceiveFrom, buf2, sizeof(buf2), 0, (SOCKADDR *) &discardTmp, &discardTmpLength);
            }
        }
    }
}

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void client_routine_tcp(client_args args) {
    string ID = args.clientID;
    auto listenOn = args.listenPort;
    auto sendTo = args.nextPort;

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);

    SOCKET socketSendTo;
    SOCKADDR_IN addrSendTo;

    SOCKET server;
    SOCKADDR_IN addrServer;

    SOCKET socketReceiveFrom;
    SOCKADDR_IN addrReceiveFrom;

    SOCKET socketBackdoor;
    SOCKADDR_IN addrBackdoor;

    SOCKET socketLoggers;
    SOCKADDR_IN addrLoggers;
    if (!args.newClient) {

        if (args.i == 0) {
            // configure backdoor socket
            socketBackdoor = socket(AF_INET, SOCK_STREAM, 0);
            addrBackdoor.sin_addr.s_addr = INADDR_ANY;
            addrBackdoor.sin_family = AF_INET;
            addrBackdoor.sin_port = args.backdoorPort;
            u_long iMode = 1;
            ioctlsocket(socketBackdoor, FIONBIO, &iMode);

            bind(socketBackdoor, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));
            listen(socketBackdoor, 0);
        }

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
            string toSend = args.nextClientID + "#0#Hello from " + ID;
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            send(socketSendTo, buf1, sizeof(buf1), 0);
        }
    } else {
        // new client
        socketBackdoor = socket(AF_INET, SOCK_STREAM, 0);
        addrBackdoor.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrBackdoor.sin_family = AF_INET;
        addrBackdoor.sin_port = sendTo;
        connect(socketBackdoor, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));

        stringstream ss;
        ss << ID << "#" << args.listenPort;
        string toSend = ss.str();
        char buf1[1024];
        strcpy(buf1, toSend.c_str());
        send(socketBackdoor, buf1, sizeof(buf1), 0);

        char buf2[1024];
        recv(socketBackdoor, buf2, sizeof(buf2), 0);

        char *id = strtok(buf2, "#");
        char *msg = strtok(nullptr, "#");

        args.nextClientID = id;
        closesocket(socketBackdoor);


        socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = stoi(msg);

        connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

        server = socket(AF_INET, SOCK_STREAM, 0);
        addrServer.sin_addr.s_addr = INADDR_ANY;
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = listenOn;

        bind(server, (SOCKADDR *) &addrServer, sizeof(addrServer));
        listen(server, 0);

        int clientAddrSize = sizeof(addrReceiveFrom);
        socketReceiveFrom = accept(server, (SOCKADDR *) &addrReceiveFrom, &clientAddrSize);

        cout << "New client successfully added." << endl;

    }

    addrLoggers.sin_addr.s_addr = inet_addr("224.0.0.3");
    addrLoggers.sin_family = AF_INET;
    addrLoggers.sin_port = htons(MULTICAST_PORT);
    socketLoggers = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketLoggers, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));
    setsockopt(socketLoggers, IPPROTO_IP, IP_MULTICAST_TTL, nullptr, 0);

    while (true) {
        char buf2[1024];
        recv(socketReceiveFrom, buf2, sizeof(buf2), 0);

        char *id = strtok(buf2, "#");
        string type = strtok(nullptr, "#");
        char *msg = strtok(nullptr, "#");

        if (ID == id) {
            stringstream sstream;
            sstream << ID << " received msg: " << msg << endl;
            char buf7[1024];
            strcpy(buf7, sstream.str().c_str());
            sendto(socketLoggers, buf7, sizeof(buf7), 0, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));

            this_thread::sleep_for(chrono::milliseconds(1000));

            if (type == "0") {
                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                send(socketSendTo, buf1, sizeof(buf1), 0);
            } else {
                string newNextID = strtok(msg, "$");
                string newNextPortString = strtok(nullptr, "$");

                args.nextClientID = newNextID;
                args.nextPort = stoi(newNextPortString);
                closesocket(socketSendTo);

                addrSendTo.sin_port = args.nextPort;
                socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
                connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                send(socketSendTo, buf1, sizeof(buf1), 0);
            }

        } else {
            stringstream ss;
            ss << id << "#" << type << "#" << msg;
            string toSend = ss.str();
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            send(socketSendTo, buf1, sizeof(buf1), 0);
        }

        if (args.i == 0) {
            SOCKADDR_IN addrNewClient;
            int newClientAddrSize = sizeof(addrNewClient);
            SOCKET socketNewClient = accept(socketBackdoor, (SOCKADDR *) &addrNewClient, &newClientAddrSize);
            if (socketNewClient != INVALID_SOCKET) {
                // there is a new client
                char buf3[1024];
                recv(socketNewClient, buf3, sizeof(buf3), 0);

                char *newClientID = strtok(buf3, "#");
                char *newClientListenPort = strtok(nullptr, "#");

                string toSend1 = args.lastClientID + "#1#" + newClientID + "$" + newClientListenPort;
                char buf1[1024];
                strcpy(buf1, toSend1.c_str());
                send(socketSendTo, buf1, sizeof(buf1), 0);

                args.lastClientID = newClientID;

                stringstream ss;
                ss << ID << "#" << args.listenPort;
                string toSend2 = ss.str();
                char buf2[1024];
                strcpy(buf2, toSend2.c_str());
                send(socketNewClient, buf2, sizeof(buf2), 0);
                closesocket(socketNewClient);

                closesocket(socketReceiveFrom);
                int clientAddrSize = sizeof(addrReceiveFrom);
                socketReceiveFrom = accept(server, (SOCKADDR *) &addrReceiveFrom, &clientAddrSize);
            }
        }

    }
}

#pragma clang diagnostic pop

int main(int argc, char **argv) {
    int maxNumberOfClients = atoi(argv[1]);
    if (maxNumberOfClients < 2) {
        cout << "Wrong number of clients" << endl;
        return 1;
    }

    int protocol = atoi(argv[2]);
    if (protocol != 1 && protocol != 2) {
        cout << "Wrong protocol" << endl;
        return 1;
    }

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    auto clients = new thread[maxNumberOfClients];
    auto clientArgs = new client_args[maxNumberOfClients];

    if (protocol == 1) {
        for (int i = 0; i < 3; i++) {
            string clientPortString;

            if (i != 0) {
                cout << "Enter ClientID and ClientPort" << endl;
                cin >> clientArgs[i].clientID >> clientPortString;
            } else {
                string backdoorPortString;
                cout << "Enter ClientID, ClientPort and BackdoorPort" << endl;
                cin >> clientArgs[i].clientID >> clientPortString >> backdoorPortString;
                clientArgs[i].backdoorPort = htons(stoi(backdoorPortString));
            }
            clientArgs[i].listenPort = htons(stoi(clientPortString));
            clientArgs[i].i = i;

        }

        clientArgs[0].nextPort = clientArgs[1].listenPort;
        clientArgs[0].nextClientID = clientArgs[1].clientID;
        clientArgs[0].hasToken = true;
        clientArgs[0].lastClientID = clientArgs[2].clientID;
        clientArgs[0].newClient = false;
        clientArgs[1].nextPort = clientArgs[2].listenPort;
        clientArgs[1].nextClientID = clientArgs[2].clientID;
        clientArgs[1].hasToken = false;
        clientArgs[1].newClient = false;
        clientArgs[2].nextPort = clientArgs[0].listenPort;
        clientArgs[2].nextClientID = clientArgs[0].clientID;
        clientArgs[2].hasToken = false;
        clientArgs[2].newClient = false;
        clients[0] = thread(client_routine_tcp, clientArgs[0]);
        clients[1] = thread(client_routine_tcp, clientArgs[1]);
        clients[2] = thread(client_routine_tcp, clientArgs[2]);

        for (int i = 3; i < maxNumberOfClients; i++) {
            string clientPortString;

            cout << "Enter ClientID and ClientPort" << endl;
            cin >> clientArgs[i].clientID >> clientPortString;

            clientArgs[i].listenPort = htons(stoi(clientPortString));
            clientArgs[i].i = i;
            clientArgs[i].nextPort = clientArgs[0].backdoorPort;
            clientArgs[i].nextClientID = clientArgs[0].clientID;
            clientArgs[i].hasToken = false;
            clientArgs[i].newClient = true;
            clients[i] = thread(client_routine_tcp, clientArgs[i]);
        }

        cout << "Maximum number of clients reached" << endl;

        for (int i = 0; i < maxNumberOfClients; i++) {
            clients[i].join();
        }
    } else {
        for (int i = 0; i < 3; i++) {
            string clientPortString;

            if (i != 0) {
                cout << "Enter ClientID and ClientPort" << endl;
                cin >> clientArgs[i].clientID >> clientPortString;
            } else {
                string backdoorPortString;
                cout << "Enter ClientID, ClientPort and BackdoorPort" << endl;
                cin >> clientArgs[i].clientID >> clientPortString >> backdoorPortString;
                clientArgs[i].backdoorPort = htons(stoi(backdoorPortString));
            }
            clientArgs[i].listenPort = htons(stoi(clientPortString));
            clientArgs[i].i = i;

        }

        clientArgs[0].nextPort = clientArgs[1].listenPort;
        clientArgs[0].nextClientID = clientArgs[1].clientID;
        clientArgs[0].hasToken = true;
        clientArgs[0].lastClientID = clientArgs[2].clientID;
        clientArgs[0].newClient = false;
        clientArgs[1].nextPort = clientArgs[2].listenPort;
        clientArgs[1].nextClientID = clientArgs[2].clientID;
        clientArgs[1].hasToken = false;
        clientArgs[1].newClient = false;
        clientArgs[2].nextPort = clientArgs[0].listenPort;
        clientArgs[2].nextClientID = clientArgs[0].clientID;
        clientArgs[2].hasToken = false;
        clientArgs[2].newClient = false;
        clients[0] = thread(client_routine_udp, clientArgs[0]);
        clients[1] = thread(client_routine_udp, clientArgs[1]);
        clients[2] = thread(client_routine_udp, clientArgs[2]);

        for (int i = 3; i < maxNumberOfClients; i++) {
            string clientPortString;

            cout << "Enter ClientID and ClientPort" << endl;
            cin >> clientArgs[i].clientID >> clientPortString;

            clientArgs[i].listenPort = htons(stoi(clientPortString));
            clientArgs[i].i = i;
            clientArgs[i].nextPort = clientArgs[0].backdoorPort;
            clientArgs[i].nextClientID = clientArgs[0].clientID;
            clientArgs[i].hasToken = false;
            clientArgs[i].newClient = true;
            clients[i] = thread(client_routine_udp, clientArgs[i]);
        }

        cout << "Maximum number of clients reached" << endl;

        for (int i = 0; i < maxNumberOfClients; i++) {
            clients[i].join();
        }
    }

    delete[] clients;
    delete[] clientArgs;
    return 0;
}