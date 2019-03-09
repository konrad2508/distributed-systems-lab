#include <iostream>
#include <winsock2.h>
#include <thread>
#include <chrono>
#include <sstream>

using namespace std;

struct client_args {
    string clientID;
    int listenPort;
    int nextPort;
    bool hasToken;
    int selectedProtocol; // TCP - 1, UDP - 2
};

void client_routine(client_args args) {
    cout << args.clientID << endl << args.listenPort << endl << args.nextPort << endl << args.hasToken << endl << args.selectedProtocol << endl << endl;



}

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
        ss << "client" << i;
        clientArgs.clientID = ss.str();

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
    return 0;
}