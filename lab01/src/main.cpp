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
        // configure original client of the ring
        if (args.i == 0) {
            // configure backdoor socket for the first client
            addrBackdoor.sin_addr.s_addr = INADDR_ANY;
            addrBackdoor.sin_family = AF_INET;
            addrBackdoor.sin_port = args.backdoorPort;

            // initialize backdoor socket as a nonblocking
            socketBackdoor = socket(AF_INET, SOCK_DGRAM, 0);
            u_long iMode = 1;
            ioctlsocket(socketBackdoor, FIONBIO, &iMode);
            bind(socketBackdoor, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));
        }

        // configure listening and forwarding sockets
        addrReceiveFrom.sin_addr.s_addr = INADDR_ANY;
        addrReceiveFrom.sin_family = AF_INET;
        addrReceiveFrom.sin_port = listenOn;

        socketReceiveFrom = socket(AF_INET, SOCK_DGRAM, 0);
        bind(socketReceiveFrom, (SOCKADDR *) &addrReceiveFrom, sizeof(addrReceiveFrom));

        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = sendTo;
        socketSendTo = socket(AF_INET, SOCK_DGRAM, 0);

        // give some time for the others to ready up
        this_thread::sleep_for(chrono::milliseconds(500));

        if (args.hasToken) {
            // first client is responsible for starting the token circulation
            string toSend = args.nextClientID + "#0#Hello from " + ID;
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
        }
    } else {
        // configure new client of the ring
        // connect through the backdoor to the first client
        addrBackdoor.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrBackdoor.sin_family = AF_INET;
        addrBackdoor.sin_port = sendTo;
        socketBackdoor = socket(AF_INET, SOCK_DGRAM, 0);

        // send init message containing new client's id and listening port to first client through the backdoor
        stringstream ss;
        ss << ID << "#" << args.listenPort;
        string toSend = ss.str();
        char buf1[1024];
        strcpy(buf1, toSend.c_str());
        sendto(socketBackdoor, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));

        // receive first client's id and his listening port
        char buf2[1024];
        SOCKADDR_IN tmp;
        int tmpLength = sizeof(tmp);
        recvfrom(socketBackdoor, buf2, sizeof(buf2), 0, (SOCKADDR *) &tmp, &tmpLength);

        char *id = strtok(buf2, "#");
        char *msg = strtok(nullptr, "#");

        args.nextClientID = id;
        closesocket(socketBackdoor);

        // configure out and in sockets
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

    // configure socket for logging
    addrLoggers.sin_addr.s_addr = inet_addr("224.0.0.3");
    addrLoggers.sin_family = AF_INET;
    addrLoggers.sin_port = htons(MULTICAST_PORT);
    socketLoggers = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketLoggers, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));
    setsockopt(socketLoggers, IPPROTO_IP, IP_MULTICAST_TTL, nullptr, 0);

    // client loop
    while (true) {
        char buf2[1024];
        SOCKADDR_IN tmp;
        int tmpLength = sizeof(tmp);
        recvfrom(socketReceiveFrom, buf2, sizeof(buf2), 0, (SOCKADDR *) &tmp, &tmpLength);

        char *id = strtok(buf2, "#");
        string type = strtok(nullptr, "#");
        char *msg = strtok(nullptr, "#");

        // check whether message was for the client
        if (ID == id) {
            // send the message to the loggers
            stringstream sstream;
            sstream << ID << " received msg: " << msg << endl;
            char buf7[1024];
            strcpy(buf7, sstream.str().c_str());
            sendto(socketLoggers, buf7, sizeof(buf7), 0, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));

            // artificially process the token
            this_thread::sleep_for(chrono::milliseconds(1000));

            if (type == "0") {
                // normal message
                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
            } else {
                // init message - client is the 'last client' in the ring - has to change forwarding socket
                string newNextID = strtok(msg, "$");
                string newNextPortString = strtok(nullptr, "$");
                args.nextClientID = newNextID;
                args.nextPort = stoi(newNextPortString);
                addrSendTo.sin_port = args.nextPort;

                // pass the message to the new client thus continuing token circulation
                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
            }

        } else {
            // pass the message and do nothing
            stringstream ss;
            ss << id << "#" << type << "#" << msg;
            string toSend = ss.str();
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
        }

        // first client has to check if someone new is trying to enter the ring
        if (args.i == 0) {
            SOCKADDR_IN addrNewClient;
            int lengthAddrNewClient = sizeof(addrNewClient);
            char buf3[1024];
            int readBytes = recvfrom(socketBackdoor, buf3, sizeof(buf3), 0, (SOCKADDR *) &addrNewClient,
                                     &lengthAddrNewClient);
            if (readBytes != SOCKET_ERROR) {
                // received message through nonblocking socket - new client wants to join
                char *newClientID = strtok(buf3, "#");
                char *newClientListenPort = strtok(nullptr, "#");

                // inform last client of the originial ring to change next host
                string toSend1 = args.lastClientID + "#1#" + newClientID + "$" + newClientListenPort;
                char buf1[1024];
                strcpy(buf1, toSend1.c_str());
                sendto(socketSendTo, buf1, sizeof(buf1), 0, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

                args.lastClientID = newClientID;

                // inform new client about first client's id and listening port
                stringstream ss;
                ss << ID << "#" << args.listenPort;
                string toSend2 = ss.str();
                char buf2[1024];
                strcpy(buf2, toSend2.c_str());
                sendto(socketBackdoor, buf2, sizeof(buf2), 0, (SOCKADDR *) &addrNewClient, lengthAddrNewClient);

                // discard one message to avoid doubling the token
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
        // configure original ring's clients
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

        // to properly connect clients, initialize out and in sockets in a cross manner - 'even' clients first
        // configure out socket then in, 'odd' clients configure in socket first
        if (args.i % 2 == 0) {
            // configure out socket
            socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
            addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
            addrSendTo.sin_family = AF_INET;
            addrSendTo.sin_port = sendTo;

            connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

            // configure in socket
            server = socket(AF_INET, SOCK_STREAM, 0);
            addrServer.sin_addr.s_addr = INADDR_ANY;
            addrServer.sin_family = AF_INET;
            addrServer.sin_port = listenOn;

            bind(server, (SOCKADDR *) &addrServer, sizeof(addrServer));
            listen(server, 0);

            int clientAddrSize = sizeof(addrReceiveFrom);
            socketReceiveFrom = accept(server, (SOCKADDR *) &addrReceiveFrom, &clientAddrSize);
        } else {
            // configure in socket
            server = socket(AF_INET, SOCK_STREAM, 0);
            addrServer.sin_addr.s_addr = INADDR_ANY;
            addrServer.sin_family = AF_INET;
            addrServer.sin_port = listenOn;

            bind(server, (SOCKADDR *) &addrServer, sizeof(addrServer));
            listen(server, 0);

            int clientAddrSize = sizeof(addrReceiveFrom);
            socketReceiveFrom = accept(server, (SOCKADDR *) &addrReceiveFrom, &clientAddrSize);

            // configure out socket
            socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
            addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
            addrSendTo.sin_family = AF_INET;
            addrSendTo.sin_port = sendTo;

            connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));
        }

        if (args.hasToken) {
            // first client has to start the token circulation
            string toSend = args.nextClientID + "#0#Hello from " + ID;
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            send(socketSendTo, buf1, sizeof(buf1), 0);
        }
    } else {
        // configure new client
        // connect to the first client through the backdoor
        socketBackdoor = socket(AF_INET, SOCK_STREAM, 0);
        addrBackdoor.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrBackdoor.sin_family = AF_INET;
        addrBackdoor.sin_port = sendTo;
        connect(socketBackdoor, (SOCKADDR *) &addrBackdoor, sizeof(addrBackdoor));

        // send the first client new client's id and listening port
        stringstream ss;
        ss << ID << "#" << args.listenPort;
        string toSend = ss.str();
        char buf1[1024];
        strcpy(buf1, toSend.c_str());
        send(socketBackdoor, buf1, sizeof(buf1), 0);

        // receive first client's id and listening port
        char buf2[1024];
        recv(socketBackdoor, buf2, sizeof(buf2), 0);

        char *id = strtok(buf2, "#");
        char *msg = strtok(nullptr, "#");

        args.nextClientID = id;

        // close backdoor connection
        closesocket(socketBackdoor);

        // configure out socket
        socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
        addrSendTo.sin_addr.s_addr = inet_addr("127.0.0.1");
        addrSendTo.sin_family = AF_INET;
        addrSendTo.sin_port = stoi(msg);

        connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

        // configure in socket
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

    // configure loggers socket
    addrLoggers.sin_addr.s_addr = inet_addr("224.0.0.3");
    addrLoggers.sin_family = AF_INET;
    addrLoggers.sin_port = htons(MULTICAST_PORT);
    socketLoggers = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketLoggers, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));
    setsockopt(socketLoggers, IPPROTO_IP, IP_MULTICAST_TTL, nullptr, 0);

    // client loop
    while (true) {
        // get token
        char buf2[1024];
        recv(socketReceiveFrom, buf2, sizeof(buf2), 0);

        char *id = strtok(buf2, "#");
        string type = strtok(nullptr, "#");
        char *msg = strtok(nullptr, "#");

        // check if the message is for the client
        if (ID == id) {
            // log the message
            stringstream sstream;
            sstream << ID << " received msg: " << msg << endl;
            char buf7[1024];
            strcpy(buf7, sstream.str().c_str());
            sendto(socketLoggers, buf7, sizeof(buf7), 0, (SOCKADDR *) &addrLoggers, sizeof(addrLoggers));

            //artificially process the message
            this_thread::sleep_for(chrono::milliseconds(1000));

            if (type == "0") {
                // normal message
                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                send(socketSendTo, buf1, sizeof(buf1), 0);
            } else {
                // received init message
                string newNextID = strtok(msg, "$");
                string newNextPortString = strtok(nullptr, "$");

                // change next recipient
                args.nextClientID = newNextID;
                args.nextPort = stoi(newNextPortString);
                // close old out socket
                closesocket(socketSendTo);

                // connect to the new client
                addrSendTo.sin_port = args.nextPort;
                socketSendTo = socket(AF_INET, SOCK_STREAM, 0);
                connect(socketSendTo, (SOCKADDR *) &addrSendTo, sizeof(addrSendTo));

                // send him a normal message to resume token circulation
                string toSend = args.nextClientID + "#0#Hello from " + ID;
                char buf1[1024];
                strcpy(buf1, toSend.c_str());
                send(socketSendTo, buf1, sizeof(buf1), 0);
            }

        } else {
            // message was not for the current client; pass it and do nothing
            stringstream ss;
            ss << id << "#" << type << "#" << msg;
            string toSend = ss.str();
            char buf1[1024];
            strcpy(buf1, toSend.c_str());
            send(socketSendTo, buf1, sizeof(buf1), 0);
        }

        // first client has to check for the new connections
        if (args.i == 0) {
            SOCKADDR_IN addrNewClient;
            int newClientAddrSize = sizeof(addrNewClient);
            SOCKET socketNewClient = accept(socketBackdoor, (SOCKADDR *) &addrNewClient, &newClientAddrSize);
            if (socketNewClient != INVALID_SOCKET) {
                // accept did not block which means there is a new client
                // receive his id and listening port
                char buf3[1024];
                recv(socketNewClient, buf3, sizeof(buf3), 0);

                char *newClientID = strtok(buf3, "#");
                char *newClientListenPort = strtok(nullptr, "#");

                // pass new client's id and listening port to the last client
                string toSend1 = args.lastClientID + "#1#" + newClientID + "$" + newClientListenPort;
                char buf1[1024];
                strcpy(buf1, toSend1.c_str());
                send(socketSendTo, buf1, sizeof(buf1), 0);

                // new client becomes the last in the new ring
                args.lastClientID = newClientID;

                // send own id and listening port to the new client
                stringstream ss;
                ss << ID << "#" << args.listenPort;
                string toSend2 = ss.str();
                char buf2[1024];
                strcpy(buf2, toSend2.c_str());
                send(socketNewClient, buf2, sizeof(buf2), 0);

                // close backdoor connection with the new client
                closesocket(socketNewClient);

                // close connection with the old last client
                closesocket(socketReceiveFrom);

                // open out socket to the new client
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

    // inform system about planned winsock usage
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    auto clients = new thread[maxNumberOfClients];
    auto clientArgs = new client_args[maxNumberOfClients];

    // tcp variant
    if (protocol == 1) {
        // read info about first 3 clients
        for (int i = 0; i < 3; i++) {
            string clientPortString;

            if (i != 0) {
                // read client id and listening port
                cout << "Enter ClientID and ClientPort" << endl;
                cin >> clientArgs[i].clientID >> clientPortString;
            } else {
                // additionally read backdoor port for the first client
                string backdoorPortString;
                cout << "Enter ClientID, ClientPort and BackdoorPort" << endl;
                cin >> clientArgs[i].clientID >> clientPortString >> backdoorPortString;
                clientArgs[i].backdoorPort = htons(stoi(backdoorPortString));
            }
            clientArgs[i].listenPort = htons(stoi(clientPortString));
            clientArgs[i].i = i;

        }

        // configure out and in ports for the original clients
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

        // start clients
        clients[0] = thread(client_routine_tcp, clientArgs[0]);
        clients[1] = thread(client_routine_tcp, clientArgs[1]);
        clients[2] = thread(client_routine_tcp, clientArgs[2]);

        // read info about additional clients and start them
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

        // join clients for the sake of code cleanliness
        for (int i = 0; i < maxNumberOfClients; i++) {
            clients[i].join();
        }
    } else {
        // udp variant
        // analogically to tcp variant, read info about first 3 clients
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

        // configure client's arguments then start them
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

        // read info about additional clients and start them
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
        // join clients
        for (int i = 0; i < maxNumberOfClients; i++) {
            clients[i].join();
        }
    }

    // do cleanup for the sake of code cleanliness
    delete[] clients;
    delete[] clientArgs;
    return 0;
}